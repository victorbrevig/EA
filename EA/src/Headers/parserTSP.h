#ifndef PARSERTSP_H
#define PARSERTSP_H

#include "graph.h"

namespace Utils
{
  namespace Parser
  {
    Graph ParseTSPGraph(const std::string& filePath);
  }
}

#endif