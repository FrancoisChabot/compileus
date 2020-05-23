#!/bin/sh

find libs -regex '.*\.\(cpp\|h\)' -exec clang-format -style=file -i {} \;
find apps -regex '.*\.\(cpp\|h\)' -exec clang-format -style=file -i {} \;
