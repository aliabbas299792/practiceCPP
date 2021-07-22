#include <iostream>

void fatal_exit(std::string msg, int exit_code = -1){
  std::cerr << msg << "\n";
  std::exit(exit_code);
}

uint64_t int_pow(uint64_t n, uint64_t exp){
  if(exp == 0) return 1;
  if(n < 0 || exp < 1) fatal_exit("Incorrect powers");
  uint64_t num = 1;
  while(exp--)
    num *= n;
  return num;
}

template<typename T, int dim_sizes, int dims>
class matrix{
  T *arr = nullptr;
public:
  matrix(){
    arr = new T[int_pow(dim_sizes, dims)];
  }

  template<typename U, typename... Args>
  T &operator()(U idx, Args... args){
    if(idx >= dim_sizes) fatal_exit("Out of bounds access");

    int dim_num = dims - 1;

    int offset = int_pow(dim_sizes, dim_num) * idx;

    return get_item(offset, dim_num - 1, args...);
  }

  template<typename U, typename... Args>
  T &get_item(int offset, int dim_num, U idx, Args... args){
    if(idx >= dim_sizes) fatal_exit("Out of bounds access");

    offset += int_pow(dim_sizes, dim_num) * idx;

    return get_item(offset, dim_num - 1, args...);
  }

  template<typename U>
  T &get_item(int offset, int dim_num, U idx){
    if(idx >= dim_sizes) fatal_exit("Out of bounds access");

    offset += int_pow(dim_sizes, dim_num) * idx;

    return arr[offset];
  }
  
  ~matrix(){
    delete arr;
  }
};

int main()
{
  matrix<int, 4, 5> mat{};

  int count = 0;
  for(int i = 0; i < 4; i++){
    for(int j = 0; j < 4; j++){
      for(int k = 0; k < 4; k++){
        for(int l = 0; l < 4; l++){
          for(int m = 0; m < 4; m++){
            mat(i, j, k, l, m) = count++;
          }
        }
      }
    }
  }

  for(int i = 0; i < 4; i++){
    for(int j = 0; j < 4; j++){
      for(int k = 0; k < 4; k++){
        for(int l = 0; l < 4; l++){
          for(int m = 0; m < 4; m++){
            std::cout << mat(i, j, k, l, m) << "\n";
          }
        }
      }
    }
  }
  
  // ends up printing from 0 to 1023
}
