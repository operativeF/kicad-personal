// stdafx.h

#pragma once

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <pcb/3d_resolver.h>
#include <pcb/base.h>
#include <pcb/kicadcurve.h>
#include <pcb/kicadpad.h>
#include <pcb/kicadpcb.h>
#include <pcb/oce_utils.h>

#include <BRepBuilderAPI_MakeWire.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFApp_Application.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>

#include <wx/log.h>