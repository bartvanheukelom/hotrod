# hotrod

A game engine where both the engine and the games it runs are:
- largely written in Javascript, or a language that compiles to it, like TypeScript.
- C++ is used to access platform features (such as OpenGL) or do number crunching (such as Bullet Physics).

The Javascript is executed using the V8 runtime.

# Building

### Linux

#### Dependencies

##### V8 (version 5.0)

- https://github.com/v8/v8/wiki/Using%20Git
- https://github.com/v8/v8/wiki/Building%20with%20Gyp
- `make i18nsupport=off native -j16`
- Put resulting files in `bin`

##### Bullet

TODO

#### Hotrod

Run `make full`.
