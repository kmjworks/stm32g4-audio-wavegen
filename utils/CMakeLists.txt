# Utilities (standalone modules with singular functionality implemented in a standalone manner)

# Utility modules are expected to live in subdirectories of this folder, so they
# are included via `#include <linkedlist/linkedlist.h>
# The default build structure is for a header-only libraries.

add_library(utils INTERFACE)
target_include_directories(utils INTERFACE .)
