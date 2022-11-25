#!/bin/lua
-- Run Tests

-- ### GLOBALS ### --

local description = "Run or update one of several types of tests"

-- Update this if needed
-- TODO: check that this binary exists
local binary = "build/tc"
local testDir = "tests/"

local testActions = { "update", "run" }
local testGens = { "ast", "bytecode" }
local testGen = nil


-- ### UTILITIES ### --

-- C-like printing
local function printf(fmt, ...)
  io.write(string.format(fmt, ...))
end

-- Check if file exists
local function fileExists(filename)
  local file = io.open(filename, "r")

  if file ~= nil then
    file:close()
    return true
  else return false end
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

local function generateASTs()
  local filenames = scandir(testDir, "*.tc")

  local testOutputs = { }
  for _, filename in pairs(filenames) do
    local pfile = io.popen(binary .. " " .. filename)

    testOutputs[filename] = pfile:read("*a")

    pfile:close()
  end

  return testOutputs
end

local function getExpectedOutput()
  local ptrn = nil
  if testGen == "ast" then ptrn = "*.tc.ast.out" end

  local filenames = scandir(testDir, ptrn)

  local outputs = { }
  for _, filename in pairs(filenames) do
    local file = io.open(filename, "r")

    outputs[filename] = file:read("*a")

    file:close()
  end

  return outputs
end


-- ### UPDATE TESTS ### --

local function updateTests()
  if testGen == "ast" then
    printf("Updating AST tests...\n")

    local testOutputs = generateASTs()
    local expectedOutputs = getExpectedOutput()

    if not testOutputs then
      panic(1, "No test files")
    elseif not expectedOutputs then
      panic(1, "No output files")
    end

    local numUpdated, numFiles = 0, 0
    for name, _ in pairs(testOutputs) do
      if testOutputs[name] ~= expectedOutputs[name .. ".ast.out"] then
        printf("Updating %s.ast.out\n", name)
        local file = io.open(name .. ".ast.out", "w")

        file:write(testOutputs[name])

        file:close()
        numUpdated = numUpdated + 1
      end
      numFiles = numFiles + 1
    end

    printf("Updated %d of %d test files\n", numUpdated, numFiles)
  elseif testGen == "bytecode" then
    printf("This hasn't been implemented yet!\n")
  end
end


-- ### RUN TESTS ### --

local function runTests()
  if testGen == "ast" then
    printf("Running AST tests...\n")

    local testOutputs = generateASTs()
    local expectedOutputs = getExpectedOutput()

    if not testOutputs then
      panic(1, "No test files")
    elseif not expectedOutputs then
      panic(1, "No test output files")
    elseif #testOutputs ~= #expectedOutputs then
      panic(1, "Differing number of test and test output files")
    end

    local numOk, numFail = 0, 0
    for name, _ in pairs(testOutputs) do
      if testOutputs[name] == expectedOutputs[name .. ".ast.out"] then
        numOk = numOk + 1
        printf("[ ")
        setColor(COLATTR_RESET, COL_GREEN, COL_DEFAULT)
        printf("OK")
        resetColor()
        printf(" ] %s\n", name)
      else
        numFail = numFail + 1
        printf("[")
        setColor(COLATTR_BRIGHT, COL_RED, COL_DEFAULT)
        printf("FAIL")
        resetColor()
        printf("] %s\n", name)
      end
    end

    printf("%d succeeded, %d failed, %d total\n", numOk, numFail, numOk + numFail)
  elseif testGen == "bytecode" then
    printf("This hasn't been implemented yet!\n")
  end
end


-- ### MAIN ### --

local function printUsage(name)
  printf("USAGE: %s [update | run] [ast | bytecode]\n", name)
  printf("%s\n", description)
  printf("OPTIONS:\n")
  printf("  update -- run tests and overwrite corresponding file\n")
  printf("  run -- run tests, comparing the output to corresponding files\n")
  printf("\n")
  printf("  ast -- run tests for generated AST\n")
  printf("  bytecode -- run tests for bytecode ouput; this does nothing yet\n")
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
  local testAction = argv[1]
  local testGenIn = argv[2]

  -- Check if the test action is valid
  local actionMatch = false
  for _, action in pairs(testActions) do
    if action == testAction then actionMatch = true; break end
  end

  -- Check if the generation type is valid
  local genMatch = false
  for _, gen in pairs(testGens) do
    if gen == testGenIn then
      genMatch = true
      break
    end
  end

  -- Exit if either of the last checks failed
  if not actionMatch or not genMatch then
    printUsage(argv[0])
    panic(1, "Invalid arguments")
  end

  testGen = testGenIn

  if testAction == "run" then runTests()
  elseif testAction == "update" then updateTests() end
end

main(#arg, arg)
