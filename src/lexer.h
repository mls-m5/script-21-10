#pragma once

#include "token.h"
#include <vector>

// Split a buffer into ots smallest pieces
Tokens tokenize(std::shared_ptr<Buffer>);

Tokens tokenize(std::string source);
