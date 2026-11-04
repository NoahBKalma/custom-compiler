# .nkg Language Reference

Source files use the `.nkg` extension. A program is a list of statements run top to bottom, and every statement ends with `;`.

```
print("Hello World\n");
print("this is on line 2\n");
```

## 1. Implementation status

Each feature moves through three stages: **Lexed** (the lexer produces the right tokens), **Parsed** (the parser builds an AST node), and **Compiled** (the code generator emits working assembly). A feature is only usable once it is compiled.

| Feature | Lexed | Parsed | Compiled | Notes |
|---|---|---|---|---|
| String literals | yes | yes | yes | Single or double quotes |
| `print(...)` | yes | yes | partial | String literal as first argument only |
| Number literals | yes | yes | no | Integers and decimals |
| Identifiers | yes | yes | no | No way to declare variables yet |
| Constants (`PI`) | yes | yes | no | `PI` is 3.14 |
| `+` `-` `*` `/` | yes | yes | no | |
| Unary minus | yes | yes | no | |
| Parentheses (grouping) | yes | yes | n/a | Only affects parsing |
| `==` `!=` `<=` `>=` `&&` `\|\|` | yes | no | no | Lexed, parser rejects them |
| Other operators (`^ ! % & \| = . < >`) | yes | no | no | |
| Braces `{ }` and brackets `[ ]` | yes | no | no | |
| Comments | no | no | no | |

When you add a feature, add a row here and update the sections below if it changes the syntax.

## 2. Syntax

- **Statements:** an expression followed by `;`. Empty statements (`;;`) are allowed. Whitespace and newlines are ignored.
- **Strings:** text in matching `"` or `'` quotes. Escapes like `\n` are passed through to the assembler.
- **Numbers:** digits with at most one `.` (`7`, `3.14`).
- **Identifiers:** letters, digits, and `_`, not starting with a digit.
- **Operators:** unary `-` binds tightest, then `*` `/`, then `+` `-`. All binary operators are left-associative, and parentheses override precedence.
- **Exit:** the program prints one newline when it finishes and exits with status 0.

## 3. Built-in functions

### print(value, ...)

Prints a string to the console. At least one argument is required. Only the first is used, and it must be a string literal. Extra arguments are parsed and ignored.

```
print("Hello World\n");
```

## 4. Grammar

Notation: `{ x }` means zero or more repetitions of `x`, `a | b` means "a or b", and text in quotes is a literal character. Uppercase names are single tokens from the lexer.

```
program     = { statement }
statement   = expression ";" | ";"
expression  = term { ("+" | "-") term }
term        = final { ("*" | "/") final }
final       = NUMBER
            | STRING
            | CONSTANT
            | IDENTIFIER
            | "(" expression ")"
            | "-" final
            | FUNCTION "(" expression { "," expression } ")"
```