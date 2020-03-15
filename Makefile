all: libharmony-basic.a libharmony-math.a libharmony-sfml.a
clean:
	rm libharmony-basic.a libharmony-math.a libharmony-sfml.a arg_parser/template_version/arg_parser.o config_reader/config_reader.o logger/logger.o math/arb_int/arb_int.o math/rational/rational.o sfml/vector_visual/2D_vector_height.o sfml/gui/gui.o

# BASIC LIBRARY

libharmony-basic.a: arg_parser/template_version/arg_parser.o config_reader/config_reader.o logger/logger.o
	ar -crs libharmony-basic.a arg_parser/template_version/arg_parser.o config_reader/config_reader.o logger/logger.o

arg_parser/template_version/arg_parser.o: arg_parser/template_version/arg_parser.cpp
	g++ -c arg_parser/template_version/arg_parser.cpp -o arg_parser/template_version/arg_parser.o -std=c++1z

config_reader/config_reader.o: config_reader/config_reader.cpp
	g++ -c config_reader/config_reader.cpp -o config_reader/config_reader.o

logger/logger.o: logger/logger.cpp
	g++ -c logger/logger.cpp -o logger/logger.o


# MATH LIBRARY

libharmony-math.a: math/arb_int/arb_int.o math/rational/rational.o
	ar -crs libharmony-math.a math/arb_int/arb_int.o math/rational/rational.o

math/arb_int/arb_int.o: math/arb_int/arb_int.cpp
	g++ -c math/arb_int/arb_int.cpp -o math/arb_int/arb_int.o

math/rational/rational.o: math/rational/rational.cpp
	g++ -c math/rational/rational.cpp -o math/rational/rational.o -std=c++1z


# SFML LIBRARY

libharmony-sfml.a: sfml/vector_visual/2D_vector_height.o sfml/gui/gui.o
	ar -crs libharmony-sfml.a sfml/vector_visual/2D_vector_height.o sfml/gui/gui.o

sfml/vector_visual/2D_vector_height.o: sfml/vector_visual/2D_vector_height.cpp
	g++ -c sfml/vector_visual/2D_vector_height.cpp -o sfml/vector_visual/2D_vector_height.o

sfml/gui/gui.o: sfml/gui/gui.cpp
	g++ -c sfml/gui/gui.cpp -o sfml/gui/gui.o -std=c++1z
