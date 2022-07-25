// Open a file and print its contents
include stdio
include fileio
include stat
include memory

main: string[] args -> int32
  if len args != 2 do
    stdio.eprintln "Need two arguments, got %d" len args
    return 1.
  end

  filepath: string = args[1]
  err: Result<void> = undefined

  file: Result<fileio.File*> = fileio.openFile filepath "r"
  if Error msg: string = err // Gets the Error value(a string) if it exists
    eprintln "Couldn't open file: %s" msg
    return 2
  end

  statbuf: struct stat.Stat = undefined
  err = stat.stat filepath &statbuf
  if Error msg: string = err // A bit more verbose than Rusts' "if let", but
    eprintln "Couldn't stat file: %s" msg // Rust has type inferencing and
    return 3                              // I don't lol
  end
  filesize: int32 = statbuf.st_size

  content: Result<string> = memory.allocate filesize
  if Error msg: string = content // This one is overkill tbh
    eprintln "Couldn't allocate memory: %s" msg
    return 4
  end

  err = fileio.readFile file filesize content
  if Error msg: string = err
    eprintln "Couldn't read file: %s" msg
    return 5
  end

  err = println "%s" content // I still can't believe this returns an error in C
  if Error msg: string = err // Rust just crashes if this errors
    eprintln "Couldn't read file: %s" msg
    return 6
  end

  err = memory.free content
  if Error msg: string = err
    eprintln "Couldn't free buffer memory: %s" msg // yikes D:
    return 7
  end

  err = fileio.closeFile file
  if Error msg: string = err
    eprintln "Couldn't close file: %s" msg
    return 8
  end
end
