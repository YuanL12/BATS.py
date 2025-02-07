#include "pybats.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>


namespace py = pybind11;
using namespace bats;


#define MetricInterface(M, name) py::class_<M>(m, name)\
.def(py::init<>())\
.def("__call__", (Matrix<double> (M::*)(const DataSet<double>&, const DataSet<double>&) const)(&M::operator()), "returns Matrix of pairwise distances");


#define FilteredEdgeInterface(T, name) py::class_<filtered_edge<T>>(m, name)\
.def(py::init<>())\
.def(py::init<size_t, size_t, T>());

// define filtration interfaces for metric
#define FiltrationInterface(M) \
m.def("RipsFiltration", (Filtration<double, SimplicialComplex> (*)(const DataSet<double>&, const M&, double, size_t))(&RipsFiltration));\
m.def("LightRipsFiltration", (Filtration<double, DefaultLightSimplicialComplex> (*)(const DataSet<double>&, const M&, double, size_t))(&RipsFiltration));\
m.def("RipsCoverFiltration", (Filtration<double, SimplicialComplex> (*)(const DataSet<double>&, const bats::Cover&, const M&, double, size_t))(&RipsFiltration));\
m.def("DowkerFiltration", (Filtration<double, SimplicialComplex> (*)(const DataSet<double>&, const DataSet<double>&, const M&, double, size_t))(&DowkerFiltration));\
m.def("DowkerCoverFiltration", (Filtration<double, SimplicialComplex> (*)(const DataSet<double>&, const DataSet<double>&, const M&, const bats::Cover&, double, size_t))(&DowkerFiltration));\
m.def("RipsFiltration_extension", [](const DataSet<double>& X, const M& dist, double rmax,  size_t dmax) {return RipsFiltration_extension<SimplicialComplex>(X, dist, rmax, dmax);}, "Rips Filtration with inverse map returned");\
m.def("LightRipsFiltration_extension", [](const DataSet<double>& X, const M& dist, double rmax,  size_t dmax) {return RipsFiltration_extension<DefaultLightSimplicialComplex>(X, dist, rmax, dmax);}, "Rips Filtration with inverse map returned");

// define landmark interfaces for metric
#define LandmarkInterface(M) \
m.def("greedy_landmarks", (DataSet<double> (*)(const DataSet<double>&, const size_t, const M&, const size_t))(&greedy_landmarks));\
m.def("hausdorff_landmarks", (DataSet<double> (*)(const DataSet<double>&, const double, const M&, const size_t))(&hausdorff_landmarks));\
m.def("landmark_cover", (bats::Cover (*)(const DataSet<double>&, const DataSet<double>&, const M&, const size_t))(&landmark_cover));\
m.def("landmark_eps_cover", (bats::Cover (*)(const DataSet<double>&, const DataSet<double>&, const M&, double))(&landmark_eps_cover));\
m.def("approx_center", (size_t (*)(const DataSet<double>&, const M&, size_t, size_t))(&approx_center));\
m.def("greedy_landmarks_hausdorff", (std::tuple<std::vector<size_t>, std::vector<double>> (*)(const DataSet<double>&, const M&, const size_t))(&greedy_landmarks_hausdorff));
// m.def("approx_center", (size_t (*)(const DataSet<double>&, const M&, size_t))(&approx_center));
// m.def("approx_center", (size_t (*)(const DataSet<double>&, const M&))(&approx_center), "Find index approximately in the center using an iterative landmarking procedure.");

// define discrete morozov zigzag interfaces for metric
#define dmzzInterface(M) \
m.def("DiscreteMorozovZigzag", \
	(std::tuple<SimplicialComplexDiagram, std::vector<double>> (*)(const DataSet<double>&, const M&, double, size_t))(&DiscreteMorozovZigzag), \
	"discrete Morozov Zigzag (dM-ZZ) construction." \
);\
m.def("DiscreteMorozovZigzagSets", \
	(std::tuple<Diagram<std::set<size_t>, std::vector<size_t>>, std::vector<double>> (*)(const DataSet<double>&, const M&, double))(&DiscreteMorozovZigzagSets), \
	"SetDiagram for discrete Morozov Zigzag (dM-ZZ) construction." \
);

// define RipsComplex interface
#define RipsInterface(M) \
m.def("RipsComplex",\
	(SimplicialComplex (*)(const DataSet<double>&, const M&, double, size_t))(&RipsComplex), \
	"Rips Complex constructed from data set and metric." \
);


#define MetricInterfaceAll(M, name) \
MetricInterface(M, name)\
FiltrationInterface(M)\
LandmarkInterface(M)\
dmzzInterface(M)\
RipsInterface(M)


PYBIND11_MODULE(topology, m) {

	MetricInterfaceAll(Euclidean, "Euclidean")
	MetricInterfaceAll(L1Dist, "L1Dist")
	MetricInterfaceAll(LInfDist, "LInfDist")
	MetricInterfaceAll(CosineDist, "CosineDist")
	MetricInterfaceAll(RPCosineDist, "RPCosineDist")
	MetricInterfaceAll(AngleDist, "AngleDist")
	MetricInterfaceAll(RPAngleDist, "RPAngleDist")

	FilteredEdgeInterface(double, "FilteredEdge")

	m.def("bivariate_cover", &bivariate_cover);

	// RipsFiltration on distance matrix
	m.def("RipsFiltration",
		[](const Matrix<double>& M, double r, size_t k){return RipsFiltration<SimplicialComplex>(M, r, k);},
		"Rips Filtration using built using pairwise distances."
	);
	// RipsFiltration on distance matrix
	m.def("LightRipsFiltration",
		[](const Matrix<double>& M, double r, size_t k){return RipsFiltration<DefaultLightSimplicialComplex>(M, r, k);},
		"Rips Filtration using built using pairwise distances."
	);

	// RipsFiltration on distance matrix
	m.def("RipsFiltration_extension",
		[](const Matrix<double>& M, double r, size_t k){return RipsFiltration_extension<SimplicialComplex>(M, r, k);},
		"Rips Filtration built using pairwise distances Rips with inverse map returned."
	);
	// RipsFiltration on distance matrix
	m.def("LightRipsFiltration",
		[](const Matrix<double>& M, double r, size_t k){return RipsFiltration_extension<DefaultLightSimplicialComplex>(M, r, k);},
		"Rips Filtration built using pairwise distances with inverse map returned."
	);

	m.def("RipsComplex",
		[](const Matrix<double>& M, double r, size_t k){ return RipsComplex<SimplicialComplex>(M, r, k);},
		"Rips Complex constructed from pairwise distances."
	);
	m.def("LightRipsComplex",
		[](const Matrix<double>& M, double r, size_t k){ return RipsComplex<DefaultLightSimplicialComplex>(M, r, k);},
		"Rips Complex constructed from pairwise distances."
	);


	// m.def("FlagFiltration", (Filtration<double, SimplicialComplex> (*)(std::vector<filtered_edge<double>>&, const size_t, const size_t, const double))(&FlagFiltration));
	m.def("WitnessFiltration", (Filtration<double, SimplicialComplex> (*)(const DataSet<double>&, const DataSet<double>&, const Euclidean&, double, size_t))(&WitnessFiltration));
	m.def("DowkerFiltration", (Filtration<double, SimplicialComplex> (*)(const Matrix<double>&, double, size_t))(&DowkerFiltration));
	m.def("DowkerCoverFiltration", (Filtration<double, SimplicialComplex> (*)(const Matrix<double>&, const bats::Cover&, double, size_t))(&DowkerFiltration));

	m.def("Nerve", (SimplicialComplex (*)(const bats::Cover&, const size_t))(&Nerve));

	m.def("random_landmarks", (DataSet<double> (*)(const DataSet<double>&, const size_t))(&random_landmarks));
	m.def("greedy_landmarks_hausdorff", (std::tuple<std::vector<size_t>, std::vector<double>> (*)(const Matrix<double>&, const size_t))(&greedy_landmarks_hausdorff));

	m.def("sample_sphere", (DataSet<double> (*)(const size_t, const size_t))(&sample_sphere));
	m.def("force_repel_rp", (void (*)(DataSet<double>&, double))(&force_repel_rp));

	// demo extra
	m.def("SierpinskiDiagram",
		&serpinski_diagram,
		"Diagram of Sierpinski triangle iterations."
	);
}
