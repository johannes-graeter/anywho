# design of error
- interface is enforced by a concept that enforces operators to compare error classes and instances, print the error message
- on implementing an error a static array of chars is given that says what has gone wrong
- form that a static hash is generated that serves as error uid
- errors can be chained so that we can track down what is the root cause of a surfacing error. this is implemented by a parent error instance that is owned by the error itself.
- keep all memory stack allocated for speed.

# tools
## Anyhow !-operator
!-operator like error handling is a killer feature since it is so fast and easy. To implement sth like that in c++  IMO only Macros can do that (although I don't like MACROS) since we need to return the error when std::unexpected happens. So far I also only found means to do that with a gcc extension that is unavailable to clang

Possible Names:
- ANYWHO()
- PROPAGATE()
- PROP()
- P()
- VALUE()
- V()
## adding context
We want to be able to add context to errors so we can give more informatino why the error happened.
we will add the info:
- Message
- line
- file
we decide to not use dynamic strings but static char arrays with a max size of 128. Hence memory can still be allocated on the stack keeping it efficient.
Anywho since context usually gets concatenated, longer messages are too verbose.
### syntax possibilities
1. 
```cpp
 // std::expected<std::string, IOError> readFile(const std::string path);
 std::string content = ANYWHO(readFile(path).transform_error([](const auto& error){return error.add_context(fmt::format("Failed to read instrs from {}", path)})));
```
This is too long.

2.
```cpp
 // std::expected<std::string, IOError> readFile(const std::string path);
 std::string content = ANYWHO(readFile(path).with_context(fmt::format("Failed to read instrs from {}", path)));
```
would be better very similar to rust anyhow.
We could Define `anywho::Result<Type, Error>` that defines with_context and casts to `std::expected<Type,Error>`.

3. Add function
```cpp
 // std::expected<std::string, IOError> readFile(const std::string path);
 std::string content = ANYWHO(with_context(readFile(path), fmt::format("Failed to read instrs from {}", path)));
```
4. Use context class instead of string to save also line and file

# Q&A
- Q: Why not use a AnyError abstract class as interface A: Possible, but I shun the overhead of v-tables and the nightmare that abundant inheritance can create.
- Q: Does ithappen often enough that one problem causes several errors so that we can not afford the copy? A: ?
- 
