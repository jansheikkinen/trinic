" Vim syntax file
" Language: Trinic
" Maintainer: Jans Heikkinen

" Usage: Simply edit the path in the update-syntax-vim script and run it to
" automatically place relevant language files in that path. Run the script
" again any time one of the files is changed. Note that the script will ask you
" before overwriting any existing files, so please be sure that it isn't
" overwriting anything important before you confirm it

if exists("b:current_syntax")
  finish
endif

syn case match

syn match trinicConstant "[A-Z_][A-Z0-9_]*"
syn match trinicIdentifier "[a-zA-Z_][a-zA-Z0-9_]*"

syn region trinicString start="\"" end="\""
syn region trinicChar   start="\'" end="\'"

syn match trinicNumber "\<\d\+\>"
syn match trinicNumber "\<\d\+\.\d*\>"

syn keyword trinicOperator and or not
syn match   trinicOperator "[!#?@<>=~^&|*/%+-]\|\.\{2,3}"
syn keyword trinicConstant undefined true false

syn keyword trinicType int8   int16   int32   int64   isize
                     \ uint8  uint16  uint32  uint64  usize
                     \ float8 float16 float32 float64 fsize
                     \ byte   bool    char    void    mut
                     \ any

syn keyword trinicStructure struct enum union sum trait
syn keyword trinicStatement if else while loop for do end let where in match
  \ function impl include continue return break extern as

syn keyword trinicTodo contained TODO FIXME NOTE
syn keyword trinicComment please
syn match   trinicComment "//.*$" contains=trinicTodo
syn region  trinicComment start="/\*" end="\*/"

hi def link trinicConstant   Constant
hi def link trinicIdentifier Identifier
hi def link trinicString     String
hi def link trinicChar       String
hi def link trinicNumber     Number
hi def link trinicOperator   Operator
hi def link trinicConstant   Constant
hi def link trinicType       Type
hi def link trinicStructure  Structure
hi def link trinicStatement  Statement
hi def link trinicTodo       Todo
hi def link trinicComment    Comment

let b:current_syntax = "trinic"
