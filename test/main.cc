

int main()
{
    mgl::init_opengl();
    //data
    load_data();
    claim_memory();

    Sound_Device sound_device = platform_create_sound_device();



    main_loop();
}

void init_opengl() //bool?
{
    platform_window = platform_create_window();


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