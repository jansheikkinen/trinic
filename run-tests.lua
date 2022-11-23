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


-- ### AST GENERATION ### --

local function generateASTs()
  local filenames = scandir(testDir, "*.tc")

  local testOutputs = { }
  for _, filename in pairs(filenames) do
    printf("Generating AST for %s...\n", filename)
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

    if not testOutputs then
      printf("ERROR: No test files\n")
    end

    local numFiles = 0
    for name, output in pairs(testOutputs) do
      printf("Updating %s.out\n", name)
      local file = io.open(name .. ".ast.out", "w")

      file:write(output)

      file:close()
      numFiles = numFiles + 1
    end

    printf("Updated %d test files\n", numFiles)
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
      printf("ERROR: No test files\n")
    elseif not expectedOutputs then
      printf("ERROR: No test out files\n")
    elseif #testOutputs ~= #expectedOutputs then
      printf("ERROR: Differing number of test and test out files")
      -- TODO: Exit
    end

    local numOk, numFail = 0, 0
    -- TODO: Make sure this works
    for name, _ in pairs(testOutputs) do
      if testOutputs[name] == expectedOutputs[name .. ".ast.out"] then
        numOk = numOk + 1
        printf("[ OK ] %s\n", name)
      else
        numFail = numFail + 1
        printf("[FAIL] %s\n", name)
      end
    end

    printf("%d succeeded, %d failed\n", numOk, numFail)
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
  -- Exit with code 1
end

local function main(argc, argv)
  -- Ensure correct number of arguments
  if argc ~= 2 then
    printf("ERROR: Incorrect number of arguments\n")
    printUsage(argv[0])
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
    printf("ERROR: Incorrect arguments\n")
    printUsage(argv[0])
  end

  testGen = testGenIn

  if testAction == "run" then runTests()
  elseif testAction == "update" then updateTests() end
end

main(#arg, arg)
