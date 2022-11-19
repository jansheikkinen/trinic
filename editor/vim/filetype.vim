" Trinic Filetype

if exists("did_load_filetype")
  finish
endif

au BufRead,BufNewFile *.tc set filetype=trinic
