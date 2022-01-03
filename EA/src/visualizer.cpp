#include "pch.h"
#include "visualizer.h"
#include "shader.h"
#include "vertexArray.h"
#include "vertexBuffer.h"

Visualizer::Visualizer(const Graph& graph, const TSPpermutation& permutation)
  : m_Graph(graph), m_Permutation(permutation) {};

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

int Visualizer::StartVisualization()
{
  if (!glfwInit())
    return -1;

  GLFWwindow* window;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);

  int width = 1600;
  int height = 900;

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

  Utils::BoundingBox boundingBox = m_Graph.GetBoundingBox();
  Utils::Vec2D center = boundingBox.GetCenter();
  Utils::Vec2D dimension = boundingBox.GetDimensions();

  glm::mat4 citiesTransform = glm::mat4(1.0);
  double factor = 1.8; //0.2 screen padding
  float scale = std::min((float)(1.0 / dimension.x) * factor, (float)(1.0 / dimension.y) * factor);
  citiesTransform = glm::scale(citiesTransform, glm::vec3(scale, -scale, 1.0f));
  //citiesTransform = glm::scale(citiesTransform, glm::vec3(10.0f, 10.0f, 1.0f));
  //citiesTransform = glm::translate(citiesTransform, glm::vec3(0.5f, 0.5f, 1.0f));
  citiesTransform = glm::translate(citiesTransform, glm::vec3((float)-center.x, (float)-center.y, 0.0f));

  VertexBuffer citiesVertexBuffer(citiesFloats.data(), citiesFloats.size() * sizeof(GLfloat));

  citiesVertexArray.LinkAttribute(citiesVertexBuffer, 0, 2, GL_FLOAT, 2 * sizeof(float), (void*)0);
  citiesVertexArray.Unbind();


  glm::vec3 citiesColor = glm::vec3(1.0f, 1.0f, 1.0f);
  shader.Use();
  glUniform3f(glGetUniformLocation(shader.m_ID, "color"), citiesColor.x, citiesColor.y, citiesColor.z);
  glUniformMatrix4fv(glGetUniformLocation(shader.m_ID, "model"), 1, GL_FALSE, glm::value_ptr(citiesTransform));



  glPointSize(GetPointSize());

  while (!glfwWindowShouldClose(window))
  {
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.Use();
   // camera.Matrix(m_shaderProgram, "camMatrix");
    citiesVertexArray.Bind();
    glDrawArrays(GL_POINTS, 0, citiesFloats.size() / 2);
    citiesVertexArray.Unbind();

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  return 0;
}