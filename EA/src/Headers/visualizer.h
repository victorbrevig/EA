#include "graph.h"
#include "tspPermutation.h"
#include <mutex>
#include <thread>

class Visualizer {
  const Graph& m_Graph;
  TSPpermutation m_Permutation;

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

  std::mutex m_Mutex;
  std::mutex m_PermMutex;
public:
  Visualizer(const Graph& graph, const TSPpermutation& permutation);
  int StartVisualization();
  void Inputs(GLFWwindow* window);
  void OnZoom(double amount);
  void WaitForClose();
  void WaitForSpace();
  void UpdatePermutation(const TSPpermutation& permutation);
};