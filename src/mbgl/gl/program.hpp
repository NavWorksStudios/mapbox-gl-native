#pragma once

#include <mbgl/gfx/program.hpp>
#include <mbgl/gl/types.hpp>
#include <mbgl/gl/object.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/draw_scope_resource.hpp>
#include <mbgl/gfx/vertex_buffer.hpp>
#include <mbgl/gfx/index_buffer.hpp>
#include <mbgl/gfx/uniform.hpp>
#include <mbgl/gl/vertex_array.hpp>
#include <mbgl/gl/attribute.hpp>
#include <mbgl/gl/uniform.hpp>
#include <mbgl/gl/texture.hpp>
#include <mbgl/util/io.hpp>

#include <mbgl/util/logging.hpp>
#include <mbgl/programs/program_parameters.hpp>
#include <mbgl/programs/gl/shader_source.hpp>
#include <mbgl/programs/gl/shaders.hpp>

#include <string>

namespace mbgl {
namespace gl {

template <class Name>
class Program final : public gfx::Program<Name> {
public:
    using LayoutUniformList = typename Name::LayoutUniformList;
    using SizeUniformList = typename Name::SizeUniformList;
    using PaintUniformList = typename Name::PaintUniformList;
    using AttributeList = typename Name::AttributeList;
    using TextureList = typename Name::TextureList;

    Program(ProgramParameters programParameters_)
        : programParameters(std::move(programParameters_)) {
    }

    const ProgramParameters programParameters;

    static constexpr const auto vertexOffset = programs::gl::ShaderSource<Name>::vertexOffset;
    static constexpr const auto fragmentOffset = programs::gl::ShaderSource<Name>::fragmentOffset;

    class Instance {
    public:
        Instance(Context& context,
                 const std::initializer_list<const char*>& vertexSource,
                 const std::initializer_list<const char*>& fragmentSource) :
            program(context.createProgram(
            context.createShader(ShaderType::Vertex, vertexSource),
            context.createShader(ShaderType::Fragment, fragmentSource),
            attributeLocations.getFirstAttribName())) {
            attributeLocations.queryLocations(program);
                
            layoutUniformStates.queryLocations(program);
            sizeUniformStates.queryLocations(program);
            paintUniformStates.queryLocations(program);
                
            // Texture units are specified via uniforms as well, so we need query their locations
            textureStates.queryLocations(program);
        }

        static std::unique_ptr<Instance>
        createInstance(gl::Context& context,
                       const ProgramParameters& programParameters,
                       const std::string& additionalDefines) {
            
#if 0
            // 原代码
            // Compile the shader
            const std::initializer_list<const char*> vertexSource = {
                programParameters.getDefines().c_str(),
                additionalDefines.c_str(),
                (programs::gl::shaderSource() + programs::gl::vertexPreludeOffset),
                (programs::gl::shaderSource() + vertexOffset)
            };
            
            const std::initializer_list<const char*> fragmentSource = {
                programParameters.getDefines().c_str(),
                additionalDefines.c_str(),
                (programs::gl::shaderSource() + programs::gl::fragmentPreludeOffset),
                (programs::gl::shaderSource() + fragmentOffset)
            };
#else
            const std::initializer_list<const char*> vertexSource = {
                programParameters.getDefines().c_str(),
                additionalDefines.c_str(),
                programs::gl::ShaderSource<Name>::navVertex(programs::gl::shaderSource(), programs::gl::vertexPreludeOffset),
                programs::gl::ShaderSource<Name>::navVertex(programs::gl::shaderSource())
            };
            
            const std::initializer_list<const char*> fragmentSource = {
                programParameters.getDefines().c_str(),
                additionalDefines.c_str(),
                programs::gl::ShaderSource<Name>::navFragment(programs::gl::shaderSource(), programs::gl::fragmentPreludeOffset),
                programs::gl::ShaderSource<Name>::navFragment(programs::gl::shaderSource())
            };
#endif

            
#if 0
            // shader glsl writer
            printf("\n\n******* %s - begin *******\n\n", programs::gl::ShaderSource<Name>::name);
            
            auto vit = vertexSource.begin(); vit++; vit++;
            printf("static const char* navVertex(const char* , size_t ) { return R\"(\n\n%s\n)\"; } \n\n\n", *vit); vit++;
            printf("static const char* navVertex(const char* ) { return R\"(\n\n%s\n)\"; } \n\n\n", *vit);
//            printf("###vertexSource \n%s%s%s%s\n",
//                   programParameters.getDefines().c_str(),
//                   additionalDefines.c_str(),
//                   programs::gl::ShaderSource<Name>::navVertex(programs::gl::shaderSource(), programs::gl::vertexPreludeOffset),
//                   programs::gl::ShaderSource<Name>::navVertex(programs::gl::shaderSource()));
            
            auto fit = fragmentSource.begin(); fit++; fit++;
            printf("static const char* navFragment(const char* , size_t ) { return R\"(\n\n%s\n)\"; } \n\n\n", *fit); fit++;
            printf("static const char* navFragment(const char* ) { return R\"(\n\n%s\n)\"; } \n\n\n", *fit);
//            printf("###fragmentSource \n%s%s%s%s\n",
//                   programParameters.getDefines().c_str(),
//                   additionalDefines.c_str(),
//                   programs::gl::ShaderSource<Name>::navFragment(programs::gl::shaderSource(), programs::gl::fragmentPreludeOffset),
//                   programs::gl::ShaderSource<Name>::navFragment(programs::gl::shaderSource()));
            
            printf("\n\n******* %s - end *******\n\n", programs::gl::ShaderSource<Name>::name);
#endif

            return std::make_unique<Instance>(context, vertexSource, fragmentSource);
        }

        UniqueProgram program;
        gl::UniformStates<LayoutUniformList> layoutUniformStates;
        gl::UniformStates<SizeUniformList> sizeUniformStates;
        gl::UniformStates<PaintUniformList> paintUniformStates;
        gl::AttributeLocations<AttributeList> attributeLocations;
        gl::TextureStates<TextureList> textureStates;
    };

    void draw(gfx::Context& genericContext,
              gfx::RenderPass&,
              const gfx::DrawMode& drawMode,
              const gfx::DepthMode& depthMode,
              const gfx::StencilMode& stencilMode,
              const gfx::ColorMode& colorMode,
              const gfx::CullFaceMode& cullFaceMode,
              const gfx::UniformValues<LayoutUniformList>& layoutUniforms,
              const gfx::UniformValues<PaintUniformList>& paintUniforms,
              const gfx::DrawScope& drawScope,
              const gfx::AttributeBindings<AttributeList>& attributeBindings,
              const gfx::TextureBindings<TextureList>& textureBindings,
              const gfx::IndexBuffer& indexBuffer,
              std::size_t indexOffset,
              std::size_t indexLength) override {
        auto& context = static_cast<gl::Context&>(genericContext);

        context.setDepthMode(depthMode);
        context.setStencilMode(stencilMode);
        context.setColorMode(colorMode);
        context.setCullFaceMode(cullFaceMode);

        const uint32_t key = gl::AttributeKey<AttributeList>::compute(attributeBindings);
        auto it = instances.find(key);
        if (it == instances.end()) {
            std::string&& def = gl::AttributeKey<AttributeList>::defines(attributeBindings);
            std::unique_ptr<Instance> instance = Instance::createInstance(context, programParameters, def);
            it = instances.emplace(key, std::move(instance)).first;
        }

        auto& instance = *it->second;
        context.program = instance.program;
        instance.layoutUniformStates.bind(layoutUniforms);
        instance.paintUniformStates.bind(paintUniforms);
        instance.textureStates.bind(context, textureBindings);

        auto& vertexArray = drawScope.getResource<gl::DrawScopeResource>().vertexArray;
        vertexArray.bind(context, indexBuffer, instance.attributeLocations.toBindingArray(attributeBindings));

        context.draw(drawMode, indexOffset, indexLength);
    }
    
    void draw(gfx::Context& genericContext,
              gfx::RenderPass&,
              const gfx::DrawMode& drawMode,
              const gfx::DepthMode& depthMode,
              const gfx::StencilMode& stencilMode,
              const gfx::ColorMode& colorMode,
              const gfx::CullFaceMode& cullFaceMode,
              const gfx::UniformValues<LayoutUniformList>& layoutUniforms,
              const gfx::UniformValues<SizeUniformList>& sizeUniforms,
              const gfx::UniformValues<PaintUniformList>& paintUniforms,
              const gfx::DrawScope& drawScope,
              const gfx::AttributeBindings<AttributeList>& attributeBindings,
              const gfx::TextureBindings<TextureList>& textureBindings,
              const gfx::IndexBuffer& indexBuffer,
              std::size_t indexOffset,
              std::size_t indexLength) override {
        auto& context = static_cast<gl::Context&>(genericContext);

        context.setDepthMode(depthMode);
        context.setStencilMode(stencilMode);
        context.setColorMode(colorMode);
        context.setCullFaceMode(cullFaceMode);

        const uint32_t key = gl::AttributeKey<AttributeList>::compute(attributeBindings);
        auto it = instances.find(key);
        if (it == instances.end()) {
            std::string def = gl::AttributeKey<AttributeList>::defines(attributeBindings);
            std::unique_ptr<Instance> instance = Instance::createInstance(context, programParameters, def);
            it = instances.emplace(key, std::move(instance)).first;
        }

        auto& instance = *it->second;
        context.program = instance.program;
        instance.layoutUniformStates.bind(layoutUniforms);
        instance.sizeUniformStates.bind(sizeUniforms);
        instance.paintUniformStates.bind(paintUniforms);
        instance.textureStates.bind(context, textureBindings);

        auto& vertexArray = drawScope.getResource<gl::DrawScopeResource>().vertexArray;
        vertexArray.bind(context, indexBuffer, instance.attributeLocations.toBindingArray(attributeBindings));

        context.draw(drawMode, indexOffset, indexLength);
    }
    
private:
    std::unordered_map<uint32_t, std::unique_ptr<Instance>> instances;
};

} // namespace gl
} // namespace mbgl
