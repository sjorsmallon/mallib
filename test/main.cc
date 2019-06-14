
//megabytes as int? make explicit?
//explicit in mmem
int main()
{
    // set of memory blocks?
    Memory_Pool main_pool;
    Memory_Pool asset_pool;
    Memory_Pool stack_pool;

    // merge after free?
    // defragment after free?
    mmem::request_memory(main_pool, asset_pool, Megabytes(16)); 
    mmem::request_memory(main_pool, stack_pool, Megabytes(16));
    
    mgl::init_opengl();

    //data
    // load_visual_assets();
    // load_sound_assets();
    Asset_Manager asset_manager;
    // some sort of asset table / array?
    Sound_Device sound_device = platform_create_sound_device();



    main_loop();
}

void init_opengl() //bool?
{
    Window *platform_window = platform_create_window();


}




void main_loop()
{

    while(true)
    {
        if (should_quit)
        {
            return;
        }
        do_one_frame();
    }
}