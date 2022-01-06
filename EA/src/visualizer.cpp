#include "pch.h"
#include "visualizer.h"
#include "shader.h"
#include "vertexArray.h"
#include "vertexBuffer.h"

bool Visualizer::m_HasScrollCallbacks = false;
double Visualizer::m_MouseWheelStatic = 0.0;

Visualizer::Visualizer(const Graph& graph, const TSPpermutation& permutation)
  : m_Graph(graph)
{
  m_Permutations.clear();
  m_Permutations.push_back(permutation);
  m_TranslateX = 0.0;
  m_TranslateY = 0.0;
  m_Scale = 1.0;
  m_MouseWheel = m_MouseWheelStatic;
  m_Panning = false;
  height = 0;
  width = 0;
  m_SelectedMousePosX = 0.0;
  m_SelectedMousePosY = 0.0;
  m_UpdatePermutationData = false;
  m_IsStarted = false;
  m_WaitingForSpace = false;
  m_PressingSpace = false;
};

double Visualizer::GetPointSize()
{
  if (m_Graph.GetNumberOfVertices() < 50)
    return 10.0;
  if (m_Graph.GetNumberOfVertices() < 100)
    return 8.0;
  if (m_Graph.GetNumberOfVertices() < 200)
    return 7.0;
  if (m_Graph.GetNumberOfVertices() < 500)
    return 6.0;
  if (m_Graph.GetNumberOfVertices() < 1000)
    return 5.0;
  if (m_Graph.GetNumberOfVertices() < 2500)
    return 4.0;
  if (m_Graph.GetNumberOfVertices() < 5000)
    return 3.0;
  if (m_Graph.GetNumberOfVertices() < 50000)
    return 2.0;
  return 1.0;
}

void Visualizer::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
  m_MouseWheelStatic += yoffset;
}

double Visualizer::ScreenToOpenGLX(double x)
{
  return ((double)x / (double)width * 2.0 - 1.0);
};
double Visualizer::ScreenToOpenGLY(double y)
{
  return ((double)y / (double)height * 2.0 + 1.0);
};

void Visualizer::OnZoom(GLFWwindow* window, double amount)
{
  double scaleFrac = 1.0 / 20.0;
  double scaleFactor = (1.0 + scaleFrac);
  scaleFactor = pow(scaleFactor, amount);
  m_Scale *= scaleFactor;
  m_TranslateX *= scaleFactor;
  m_TranslateY *= scaleFactor;

 /* double mouseX;
  double mouseY;
  glfwGetCursorPos(window, &mouseX, &mouseY);

  double deltaX = ScreenToOpenGLX(mouseX) - ScreenToOpenGLX(m_SelectedMousePosX);
  double deltaY = ScreenToOpenGLY(mouseY) - ScreenToOpenGLY(m_SelectedMousePosY);
  m_TranslateX -= (1 - scaleFactor) * deltaX;
  m_TranslateY -= (1 - scaleFactor) * deltaY;*/
}

void Visualizer::Inputs(GLFWwindow* window)
{
  if (!m_HasScrollCallbacks)
  {
    glfwSetScrollCallback(window, Visualizer::ScrollCallback);
    m_HasScrollCallbacks = true;
  }

  if (m_MouseWheel != m_MouseWheelStatic)
  {
    OnZoom(window, m_MouseWheelStatic - m_MouseWheel);
    m_MouseWheel = m_MouseWheelStatic;
  }

  if (m_Panning)
  {
    double mouseX;
    double mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    double deltaX = ScreenToOpenGLX(mouseX) - ScreenToOpenGLX(m_SelectedMousePosX);
    double deltaY = ScreenToOpenGLY(mouseY) - ScreenToOpenGLY(m_SelectedMousePosY);

    m_TranslateX -= deltaX;
    m_TranslateY -= deltaY;

    m_SelectedMousePosX = mouseX;
    m_SelectedMousePosY = mouseY;
  }

  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !m_Panning)
  {
    glfwGetCursorPos(window, &m_SelectedMousePosX, &m_SelectedMousePosY);
    m_Panning = true;
  }
  else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
    m_Panning = false;

  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !m_PressingSpace)
  {
    m_PressingSpace = true;
    m_WaitingForSpace = false;
  }

  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
    m_PressingSpace = false;
}

int Visualizer::StartVisualization()
{
  std::lock_guard<std::mutex> g(m_Mutex);
  if (!glfwInit())
    return -1;

  GLFWwindow* window;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);

  width = 1600;
  height = 900;

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(width, height, "Visualizer", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  gladLoadGL();

  glViewport(0, 0, width, height);

  Shader shader("Shaders\\passthrough.vert", "Shaders\\passthrough.frag");

  VertexArray citiesVertexArray;
  citiesVertexArray.Bind();

  std::vector<GLfloat> citiesFloats = m_Graph.PointsToGLFloats();
  std::vector<std::vector<GLfloat>> toursFloats;
  toursFloats.reserve(m_Permutations.size());
  for(auto& permutation : m_Permutations)
    toursFloats.emplace_back(m_Graph.PointsToGLFloats(permutation.order));

  Utils::BoundingBox boundingBox = m_Graph.GetBoundingBox();
  Utils::Vec2D center = boundingBox.GetCenter();
  Utils::Vec2D dimension = boundingBox.GetDimensions();

  VertexBuffer citiesVertexBuffer(citiesFloats.data(), citiesFloats.size() * sizeof(GLfloat));

  citiesVertexArray.LinkAttribute(citiesVertexBuffer, 0, 2, GL_FLOAT, 2 * sizeof(float), (void*)0);
  citiesVertexArray.Unbind();

  std::vector<VertexArray> tourVertexArrays;

  for (std::vector<GLfloat>& tourFloats : toursFloats)
  {

    VertexArray tourVertexArray;
    tourVertexArray.Bind();
    VertexBuffer tourVertexBuffer(tourFloats.data(), tourFloats.size() * sizeof(GLfloat));

    tourVertexArray.LinkAttribute(tourVertexBuffer, 0, 2, GL_FLOAT, 2 * sizeof(float), (void*)0);
    tourVertexArray.Unbind();

    tourVertexArrays.emplace_back(std::move(tourVertexArray));
  }



  glPointSize((GLfloat)GetPointSize());
  glLineWidth((GLfloat)std::max(GetPointSize() / 3.0, 1.0));
  glm::vec3 citiesColor = glm::vec3(1.0f, 1.0f, 1.0f);
  glm::vec3 tourColor = glm::vec3(1.0f, 0.5f, 0.25f);
  glm::vec3 tourColorSecond = glm::vec3(0.25f, 0.5f, 1.0f);

  while (!glfwWindowShouldClose(window))
  {
    m_IsStarted = true;
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Inputs(window);

    if (true)
    {
      std::lock_guard<std::mutex> g(m_PermMutex);


      toursFloats.clear();
      for (auto& permutation : m_Permutations)
        toursFloats.emplace_back(m_Graph.PointsToGLFloats(permutation.order));

      for (VertexArray& tourVertexArray : tourVertexArrays)
        tourVertexArray.DeleteArrays();
      tourVertexArrays.clear();

      for (std::vector<GLfloat>& tourFloats : toursFloats)
      {

        VertexArray tourVertexArray;
        tourVertexArray.Bind();
        VertexBuffer tourVertexBuffer(tourFloats.data(), tourFloats.size() * sizeof(GLfloat));

        tourVertexArray.LinkAttribute(tourVertexBuffer, 0, 2, GL_FLOAT, 2 * sizeof(float), (void*)0);
        tourVertexArray.Unbind();

        tourVertexArrays.emplace_back(std::move(tourVertexArray));
      }

      m_UpdatePermutationData = false;
    }

    glm::mat4 citiesTransform = glm::mat4(1.0);
    float factor = 1.8f; //0.2 screen padding
    float scale = std::min((float)(1.0 / dimension.x), (float)(1.0 / dimension.y)) * (float)m_Scale * factor;
    citiesTransform = glm::scale(citiesTransform, glm::vec3(scale, -scale, 1.0f));
    citiesTransform = glm::translate(citiesTransform, glm::vec3((float)-(center.x + m_TranslateX / scale), (float)-(center.y + m_TranslateY / scale), 0.0f));


    shader.Use();
    glUniformMatrix4fv(glGetUniformLocation(shader.m_ID, "model"), 1, GL_FALSE, glm::value_ptr(citiesTransform));
    glUniform3f(glGetUniformLocation(shader.m_ID, "color"), citiesColor.x, citiesColor.y, citiesColor.z);
    citiesVertexArray.Bind();
    glDrawArrays(GL_POINTS, 0, (GLsizei)(citiesFloats.size() / 2));
    citiesVertexArray.Unbind();

    glUniform3f(glGetUniformLocation(shader.m_ID, "color"), tourColor.x, tourColor.y, tourColor.z);

    if(tourVertexArrays.size() == 2)
      glLineWidth((GLfloat)(std::max(GetPointSize() / 3.0, 1.0) * 2.0));

    for (VertexArray& tourVertexArray : tourVertexArrays)
    {
      tourVertexArray.Bind();
      glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)(citiesFloats.size() / 2));
      tourVertexArray.Unbind();

      if (tourVertexArrays.size() == 2)
      {
        glUniform3f(glGetUniformLocation(shader.m_ID, "color"), tourColorSecond.x, tourColorSecond.y, tourColorSecond.z);
        glLineWidth((GLfloat)std::max(GetPointSize() / 3.0, 1.0));
      }
        
    }


    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  for (VertexArray& tourVertexArray : tourVertexArrays)
    tourVertexArray.DeleteArrays();

  glfwDestroyWindow(window);
  return 0;
}

void Visualizer::UpdatePermutation(const TSPpermutation& permutation)
{
  std::lock_guard<std::mutex> g(m_PermMutex);
  m_Permutations.clear();
  m_Permutations.push_back(permutation);
  m_UpdatePermutationData = true;
}

void Visualizer::UpdatePermutation(const std::vector<TSPpermutation>& permutations)
{
  std::lock_guard<std::mutex> g(m_PermMutex);
  m_Permutations = permutations;
  m_UpdatePermutationData = true;
}

void Visualizer::UpdatePermutation(const std::vector<uint32_t>& order)
{
  std::lock_guard<std::mutex> g(m_PermMutex);
  TSPpermutation newOrder(order);
  m_Permutations.clear();
  m_Permutations.push_back(newOrder);
  m_UpdatePermutationData = true;
}

void Visualizer::WaitForClose()
{
  while (!m_IsStarted);
  std::lock_guard<std::mutex> g(m_Mutex);
}

void Visualizer::WaitForSpace()
{
  m_WaitingForSpace = true;
  while (m_WaitingForSpace);
}