#ifndef RubyBlockState_h
#define RubyBlockState_h

// We store weird things in QTextBlock user state integer, I don't remember why we do this way, but we do.
// So this macros are to try to at least unify the way we manage the block state.
// Why 20? Because scanner uses 2 bits to store the state code plus 16 to store a unicode char to know
// the character used to close the string/regexp/etc... i.e. a mess
#define RUBY_BLOCK_IDENT(state) (state >> 20)
#define RUBY_BLOCK_SCANNER_STATE(state) (state & 0xfffff)
#define RUBY_BLOCK_PACK(indent, state) ((indent << 20) | state)
#endif
