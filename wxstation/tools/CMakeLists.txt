add_executable(wx-i2c wx-i2c.cpp)

include_directories(AFTER "../src") 

target_link_libraries(wx-i2c
  PRIVATE
     wxstation 
  )

#  target_compile_features(fib
#  PRIVATE
#      cxx_digit_separators
#  )
