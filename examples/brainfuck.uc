/* test.uc */

include stdIO

uint8[UINT16_MAX] CELLS

uint8 interpret(string program)
  mut uint16 cell_ptr    = 0
  mut usize  program_ptr = 0
  mut usize  depth       = 0

  char[] tokens = tochars(program)
  while program_ptr < lengthof(tokens) do
    char   token = tokens[program_ptr]
    uint8* cell  = &(CELLS[cell_ptr])

    if     token == '+' do *cell += 1
    elseif token == '-' do *cell -= 1
    elseif token == '<' do cell_ptr -= 1
    elseif token == '>' do cell_ptr += 1
    elseif token == '.' do print("%s", *cell)
    elseif token == ',' do getc(STDIN)
    elseif token == '[' and *cell == 0 do
      while program_ptr < lengthof(tokens) do
        program_ptr += 1
        token = tokens[program_ptr]

        if token == ']' do
          if depth == 0 do break
          else depth -= 1 end
        end
      end

      println("Expected matching ']' after '['")
      exit(1)
    elseif token == ']' and *cell != 0 do
      while program_ptr > 0 do
        program_ptr -= 1
        token = tokens[program_ptr]

        if token == '[' do
          if depth == 0 do break
          else depth -= 1 end
        end
      end

      println("Expected matching '[' before ']'")
      exit(1)
    end
  end
end

int32 main(string[] args)
  interpret("-.[-.]") // should print every ASCII char in reverse order
  return 0
end
