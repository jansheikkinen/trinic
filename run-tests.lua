-- Run Tests

-- ### GLOBALS ### --

local description = "Run or update one of several types of tests"

local testActions = { "update", "run" }
local testGens = { "ast", "bytecode" }
local testGen = nil


-- ### UTILITIES ### --

local function printf(fmt, ...)
  io.write(string.format(fmt, ...))
end


-- ### UPDATE TESTS ### --

local function updateTests(testdir, outdir)

end


-- ### RUN TESTS ### --

local function runTests(testdir, outdir)

end


-- ### MAIN ### --

local function printUsage(name)
  printf("USAGE: %s [update | run] [ast | bytecode] TESTDIR OUTDIR\n", name)
  printf("%s\n", description)
  printf("OPTIONS:\n")
  printf("  update -- run tests from TESTDIR and write output to OUTDIR\n")
  printf("  run -- run tests in TESTDIR, " ..
    "comparing the output to corresponding files in OUTDIRDIR\n")
  printf("\n")
  printf("  ast -- run tests for generated AST\n")
  printf("  bytecode -- run tests for bytecode ouput; this does nothing yet\n")
  -- Exit with code 1
end

local function main(argc, argv)
  -- Ensure correct number of arguments
  if argc ~= 4 then printUsage(argv[0]) end

  -- Alias arguments
  local testAction = argv[1]
  local testGenIn = argv[2]

  -- Check if the test action is valid
  local actionMatch = false
  for action in pairs(testActions) do
    if action == testAction then actionMatch = true; break end
  end

  -- Check if the generation type is valid
  local genMatch = false
  for gen in pairs(testGens) do
    if gen == testGenIn then genMatch = true; break end
  end

  -- Exit if either of the last checks failed
  if not actionMatch or not genMatch then
    printUsage(argv[0])
  end

  testGen = testGenIn
end

main(#arg, arg)
