CPP = em++
SHELL_FILE = shell.html
C_SOURCES = ${wildcard *.cpp} libxlsxwriter/src/libxlsxwriter.a
OTHER_SOURCES = ${SHELL_FILE} public/main.js
EMCC_FLAGS_ = WASM=1 NO_EXIT_RUNTIME=1 EXPORTED_RUNTIME_METHODS=['ccall'] USE_ZLIB=1 ERROR_ON_UNDEFINED_SYMBOLS=0 ALLOW_MEMORY_GROWTH=1 USE_PTHREADS=1 PTHREAD_POOL_SIZE=2
INCLUDES_ = /usr/local/include libxlsxwriter/include
OUTPUT_ = public/index.html

OUTPUT = -o ${OUTPUT_}
INCLUDES = $(foreach include, $(INCLUDES_), -I$(include))
EMCC_FLAGS = $(foreach flag, $(EMCC_FLAGS_), -s $(flag))


libxlsxwriter/src/libxlsxwriter.a libxlsxwriter/src/*.c:
	cd libxlsxwriter && emmake make

clean:
	cd libxlsxwriter && emmake make clean
	rm public/index.html
	rm public/index.js
	rm public/index.wasm

run:
	python3 -m http.server --directory public

build: ${C_SOURCES} ${OTHER_SOURCES}
	${CPP} ${C_SOURCES} ${EMCC_FLAGS} ${INCLUDES} ${OUTPUT} --shell-file ${SHELL_FILE}
