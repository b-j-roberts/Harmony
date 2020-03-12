# build basic object files
echo 'Building : arg_parser/template_version/arg_parser.o'
g++ -c arg_parser/template_version/arg_parser.cpp -o arg_parser/template_version/arg_parser.o -std=c++1z
echo 'Building : config_reader/config_reader.o'
g++ -c config_reader/config_reader.cpp -o config_reader/config_reader.o
echo 'Building : logger/logger.o'
g++ -c logger/logger.cpp -o logger/logger.o
# build basic lib
echo 'Creating static library : libharmony-basic.a'
ar -crs libharmony-basic.a arg_parser/template_version/arg_parser.o config_reader/config_reader.o logger/logger.o

# build math object files
echo 'Building : math/arb_int/arb_int.o'
g++ -c math/arb_int/arb_int.cpp -o math/arb_int/arb_int.o
echo 'Building : math/rational/rational.o'
g++ -c math/rational/rational.cpp -o math/rational/rational.o -std=c++1z
echo 'Creating static library : libharmony-math.a'
ar -crs libharmony-math.a math/arb_int/arb_int.o math/rational/rational.o


# build sfml object files
echo 'Building : sfml/vector_visual/2D_vector_height.o'
g++ -c sfml/vector_visual/2D_vector_height.cpp -o sfml/vector_visual/2D_vector_height.o
echo 'Building : sfml/gui/gui.o'
g++ -c sfml/gui/gui.cpp -o sfml/gui/gui.o -std=c++1z
echo 'Creating static library : libharmony-sfml.a'
ar -crs libharmony-sfml.a sfml/vector_visual/2D_vector_height.o sfml/gui/gui.o
