#ifndef VISUALIZER_H
#define VISUALIZER_H

#include "graph.h"
#include "tspPermutation.h"
#include <mutex>
#include <thread>

class Visualizer {
  const Graph& m_Graph;
  std::vector<TSPpermutation> m_Permutations;

  int width;
  int height;
  double m_Scale;
  double m_TranslateX;
  double m_TranslateY;
  static bool m_HasScrollCallbacks;
  static double m_MouseWheelStatic;
  double m_MouseWheel;
  bool m_Panning;
  double m_SelectedMousePosX;
  double m_SelectedMousePosY;
  bool m_PressingSpace;
  volatile bool m_UpdatePermutationData;
  volatile bool m_IsStarted;
  volatile bool m_WaitingForSpace;
  static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

  double GetPointSize();
  double ScreenToOpenGLX(double x);
  double ScreenToOpenGLY(double y);
  std::mutex m_Mutex;
  std::mutex m_PermMutex;
public:
  Visualizer(const Graph& graph, const TSPpermutation& permutation);
  int StartVisualization();
  void Inputs(GLFWwindow* window);
  void OnZoom(GLFWwindow* window, double amount);
  void WaitForClose();
  void WaitForSpace();
  void UpdatePermutation(const TSPpermutation& permutation);
  void UpdatePermutation(const std::vector<TSPpermutation>& permutations);
  void UpdatePermutation(const std::vector<uint32_t>& order);
};
#endif