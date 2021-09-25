# makefile for launch program

msys_version := $(if $(findstring Msys, $(shell uname -o)),$(word 1, $(subst ., ,$(shell uname -r))),0)
$(info The version of MSYS you are running is $(msys_version) (0 meaning not MSYS at all))

CC = gcc
ifeq ($(msys_version), 0)
	PY_EXT =
	DYNAMIC = -rdynamic
	MAIN_DYNAMIC = 
else
	PY_EXT = .exe
	DYNAMIC = -Wl,--export-all-symbols
	MAIN_DYNAMIC = resources/windows_info.res
endif
INC = -Iinclude

# Normally has debug symbols; supports 'make release' which overrides CFLAGS
CFLAGS = -Wall -Werror -Wextra -ggdb3 $(shell pkg-config --cflags gtk+-3.0)
LDFLAGS = $(shell pkg-config --libs gtk+-3.0)
ifneq ($(msys_version), 0)
	LDFLAGS += -Wl,-subsystem,windows
endif
INC = -Iinclude

# All .c files under src/
SOURCES := $(shell find src/ -name '*.c') 

# Temporary/Backup files in root and src/ directories (avoids removing backup glade files)
REMOVE := *~ $(shell find src/ -name '*~')
# Build artifacts (excluding PyInstaller)
REMOVE += $(shell find src/ -name '*.o') launch
# Cache
REMOVE += __pycache__ launch-cache
# PyInstaller build artifacts
REMOVE_PY = build dist *.spec

PY_SCRIPTS = plot
PY_TARGETS = /dist/plot/plot$(PY_EXT)

ifeq ($(msys_version), 0)
define GEN_RULE
../gtk_simple_plot/dist/$(script)/$(script)$(PY_EXT) : $(script).py
	python3.6 -m PyInstaller $(script).py -y
	rm -rf build
endef
else
define GEN_RULE
/dist/$(script)/$(script)$(PY_EXT) : $(script).py
	pyinstaller $(script).py -y -w
	rm -rf build
endef
endif

all: $(PY_TARGETS) main
	@echo done!

main: $(patsubst %.c,%.o,$(SOURCES))
# Compile the windows resource file, to be included when compiling the .exe
ifneq ($(msys_version), 0)
	@echo windows_info.rc...
	@windres resources/windows_info.rc -O coff -o resources/windows_info.res
endif
	@echo launch program...
	@$(CC) $(CFLAGS) -o launch $^ $(LDFLAGS) $(DYNAMIC) $(MAIN_DYNAMIC) -lm -no-pie

$(foreach script,$(PY_SCRIPTS), \
 	$(eval $(GEN_RULE)) \
 )

# COMPILE
%.o: %.c
	@echo $@...
	@$(CC) $(INC) -c -o $@ $(CFLAGS) $< $(LDFLAGS) $(DYNAMIC) -lm

clean:
	rm -rf $(REMOVE)
	rm -rf $(REMOVE_PY)
clean_nopy:
	rm -rf $(REMOVE)

# Override CFLAGS to not include debug information, for use in release builds
release: CFLAGS = -Wall -Werror -Wextra $(shell pkg-config --cflags gtk+-3.0)
release: all
