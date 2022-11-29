#!/bin/lua
-- Run Tests

-- ### GLOBALS ### --

local description = "Run or update one of several types of tests"

-- Update this if needed
-- TODO: check that this binary exists
local binary = "build/tc"
local testDir = "tests/"

local testTypes = { "run", "update" } -- TODO: "delete"
local test = {
  ast = {
    name = "ast",
    flags = "-da",
    ext = ".ast.out",
    desc = "run tests for generated AST"
  },
  lex = {
    name = "lex",
    flags = "-dl",
    ext = ".lex.out",
    desc = "run tests for generated tokens"
  },
  bytecode = {
    name = "bytecode",
    flags = "-db",
    ext = ".bc.out",
    desc = "run tests for bytecode output; unimplemented"
  },
  all = {
    name = "all",
    flags = "-dlab",
    ext = ".out",
    desc = "run all tests in one"
  }
}

-- ### UTILITIES ### --

-- C-like printing
local function printf(fmt, ...)
  io.write(string.format(fmt, ...))
end

local COLATTR_RESET = 0
local COLATTR_BRIGHT = 1
local COLATTR_DIM = 2
local COLATTR_ITALIC = 3
local COLATTR_UNDERLINE = 4
local COLATTR_BLINK = 5
local COLATTR_REVERSE = 7
local COLATTR_HIDDEN = 8
local COLATTR_STRIKE = 9
local COLATTR_BRIGHT_UNDERLINE = 21
local COLATTR_OVERLINE = 53

local COL_BLACK = 0
local COL_RED = 1
local COL_GREEN = 2
local COL_ORANGE = 3
local COL_BLUE = 4
local COL_MAGENTA = 5
local COL_CYAN = 6
local COL_LGREY = 7
local COL_UNDEFINED = 8
local COL_DEFAULT = 9

local COLBG_LRED = 0
local COLBG_LGREEN = 1
local COLBG_YELLOW = 2
local COLBG_LBLUE = 3
local COLBG_LPURPLE = 4
local COLBG_TEAL = 5
local COLBG_WHITE = 6

local function setColor(attr, fg, bg)
  printf("%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40)
end

local function resetColor()
  setColor(COLATTR_RESET, COL_DEFAULT, COL_DEFAULT)
end

local function printfc(fmt, attr, fg, bg, ...)
  setColor(attr, fg, bg)
  printf(fmt, ...)
  resetColor()
end

-- Get all files in a directory
local function scandir(dir, ptrn)
  local i, t = 0, { }

  local pfile = io.popen("find " .. dir .. " -type f -name \"" .. ptrn .. "\"")

  for filename in pfile:lines() do
    i = i + 1
    t[i] = filename
  end

  pfile:close()
  return t
end

-- Get diff of two files
local function diff(file1, file2)
  os.execute("diff -uN --color " .. file1 .. " " .. file2)
end

-- Check if a file or directory exists in this path
-- Stolen from Hisham H M on:
-- https://stackoverflow.com/questions/1340230/check-if-directory-exists-in-lua
local function exists(file)
  local ok, err, code = os.rename(file, file)
  if not ok then
     if code == 13 then
        -- Permission denied, but it exists
        return true
     end
  end
  return ok, err
end

-- Print message and exit with code
local function panic(err, msg)
  setColor(COLATTR_RESET, COL_RED, COL_DEFAULT)
  printf("Error")
  resetColor()
  printf(": %s\n", msg)
  os.exit(err)
end


-- ### AST GENERATION ### --

local function generateOutputs(flags)
  local filenames = scandir(testDir, "*.tc")

  local testOutputs = { }
  for _, filename in pairs(filenames) do
    local pfile = io.popen(binary .. " " .. flags .. " " .. filename)

    testOutputs[filename] = pfile:read("*a")

    pfile:close()
  end

  return testOutputs
end

local function getExpectedOutput(ext)
  local ptrn = "*.tc" .. ext

  local filenames = scandir(testDir, ptrn)

  local outputs = { }
  for _, filename in pairs(filenames) do
    local file = io.open(filename, "r")

    outputs[filename] = file:read("*a")

    file:close()
  end

  return outputs
end

local function compareOutputs(testOut, expectOut)
  if not testOut then
    panic(1, "No test files")
  elseif not expectOut then
    panic(1, "No test output files")
  elseif #testOut ~= #expectOut then
    panic(1, "Differing number of test and test output files")
  end
end


-- ### UPDATE TESTS ### --

local function updateTests(type)
  printf("Updating %s tests...\n", type.name)

  local testOut = generateOutputs(type.flags)
  local expectOut = getExpectedOutput(type.ext)

  compareOutputs(testOut, expectOut)

  local numUpdated, numFiles = 0, 0
  for name, _ in pairs(testOut) do
    local filename = name .. type.ext
    if testOut[name] ~= expectOut[filename] then
      printf("View diff for %s? (y / N) ", name)

      if io.read() == "y" then
        local tmp = io.open("/tmp/trinictest", "w")
        tmp:write(testOut[name])
        tmp:close()

        diff(filename, "/tmp/trinictest")

        os.remove("/tmp/trinictest")
        printf("\n")
      end

      printf("Update %s with new output? (Y / n) ", name)

      if io.read() ~= "n" then
        printf("Updating %s\n", filename)

        local file = io.open(filename, "w")
        file:write(testOut[name])
        file:close()

        numUpdated = numUpdated + 1
      end
    end
    numFiles = numFiles + 1
  end

  if numUpdated == 0 then printf("No files need updating\n")
  else printf("Updated %d of %d test files\n", numUpdated, numFiles) end
end


-- ### RUN TESTS ### --

local function runTests(type)
  printf("Running %s tests...\n", type.name)

  local testOut = generateOutputs(type.flags)
  local expectOut = getExpectedOutput(type.ext)

  compareOutputs(testOut, expectOut)

  local numOk, numFail = 0, 0
  for name, _ in pairs(testOut) do
    if testOut[name] == expectOut[name .. type.ext] then
      numOk = numOk + 1
      printf("[ ")
      printfc("OK", COLATTR_RESET, COL_GREEN, COL_DEFAULT)
      printf(" ] %s\n", name)
    else
      numFail = numFail + 1
      printf("[")
      printfc("FAIL", COLATTR_BRIGHT, COL_RED, COL_DEFAULT)
      printf("] %s\n", name)
    end
  end
  printf("%d succeeded, %d failed, %d total\n", numOk, numFail, numOk + numFail)
end


-- ### MAIN ### --

local function printUsage(name)
  printf("USAGE: %s [update | run] [ast | lex | bytecode]\n", name)
  printf("%s\n", description)
  printf("OPTIONS:\n")
  printf("  update -- run tests and overwrite corresponding file\n")
  printf("  run -- run tests, comparing the output to corresponding files\n")
  printf("\n")
  for fname, fields in pairs(test) do
    printf("  %s -- %s\n", fname, fields.desc)
  end
end

local function main(argc, argv)
  -- Ensure correct number of arguments
  if argc ~= 2 then
    printUsage(argv[0])
    panic(1, "Incorrect number of arguments")
  end

  -- Ensure globals exis
  local ok, err = exists(binary)
  local ok_, err_ = exists(testDir)
  ok, err = ok or ok_, err or err_
  if not ok or err then
    panic(1, "Please edit \"binary\" and \"testDir\" variables to point to correct paths")
  end

  -- Alias arguments
  local testType = argv[1]
  local testAction = argv[2]

  -- Check if the test type is valid
  local typeMatch = false
  for _, type in pairs(testTypes) do
    if type == testType then typeMatch = true; break end
  end

  -- Check if the generation action is valid
  local actionMatch = false
  for action, _ in pairs(test) do
    if action == testAction then
      actionMatch = true
      break
    end
  end

  -- Exit if either of the last checks failed
  if not typeMatch or not actionMatch then
    printUsage(argv[0])
    panic(1, "Invalid arguments")
  end

  if testType == "run" then runTests(test[testAction])
  elseif testType == "update" then updateTests(test[testAction]) end
end

main(#arg, arg)
