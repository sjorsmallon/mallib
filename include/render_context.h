#ifndef INCLUDED_RENDER_CONTEXT_
#define INCLUDED_RENDER_CONTEXT_


namespace rc
{
	struct Render_Context
	{
		uint32_t active_shader;
	}	

	set_shader(Render_Context& render_context, const char* shader_name);
	
	
}



