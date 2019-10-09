class MyTexture
{
private:
  GLuint obj_ = 0; //Cannot leave this uninitialized.

  void Release()
  {
    glDeleteTextures(1, &obj_);
    obj_ = 0;
  }

public:
  //Other constructors as normal.

  //Free up the texture.
  ~MyTexture() {Release();}

  //Delete the copy constructor/assignment.
  MyTexture(const MyTexture &) = delete;
  MyTexture &operator=(const MyTexture &) = delete;

  MyTexture(MyTexture &&other) : obj_(other.obj_)
  {
    other.obj_ = 0; //Use the "null" texture for the old object.
  }

  MyTexture &operator=(MyTexture &&other)
  {
    //ALWAYS check for self-assignment.
    if(this != &other)
    {
      Release();
      //obj_ is now 0.
      std::swap(obj_, other.obj_);
    }
  }
  
};