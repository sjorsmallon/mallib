#ifndef INCLUDE_FILE_HANDLER_
#define INCLUDE_FILE_HANDLER_

struct File_Handler
{
    size_t m_length;
    
    public:
        ~File()
        {
            free(static_cast<void *>(data));
            munmap(static_cast<void *>(data),m_length);
        };


};

#endif

