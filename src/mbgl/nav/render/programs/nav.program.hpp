
#include <cassert>


namespace nav {
namespace programs {

typedef GLint(*GetLocation)(GLuint, const GLchar*);

template <GetLocation f> struct Location {
    Location(GLint program, const char* name) {
        value = f(program, name);
        assert(value >= 0);
    }
    
    operator GLint () { return value; }
    GLint value;
};

using UniformLocation = Location<glGetUniformLocation>;
using AttribLocation = Location<glGetAttribLocation>;

} // namespace programs
} // nav
