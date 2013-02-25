SET( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

SET( OPENMOC_SRC
  Cell.cpp
  FlatSourceRegion.cpp
  Geometry.cpp
  Lattice.cpp
  LocalCoords.cpp
  log.cpp
  Material.cpp
  Mesh.cpp
  MeshCell.cpp
  MeshSurface.cpp
  openmoc.cpp
  Options.cpp
  Parser.cpp
  Plotter.cpp
  Point.cpp
  Quadrature.cpp
  Solver.cpp
  Surface.cpp
  Timer.cpp
  Track.cpp
  TrackGenerator.cpp
  Universe.cpp
)

# Build the OpenMOC executable
ADD_EXECUTABLE(openmoc ${OPENMOC_SRC})
TARGET_LINK_LIBRARIES( openmoc 
                       ${EXPAT_LIBRARIES}
                       ${Silo_LIBRARIES} 
                       ${ImageMagick_LIBRARIES} 
)

