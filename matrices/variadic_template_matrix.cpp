#include <iostream>
#include <vector>

void fatal_exit(std::string msg, int exit_code = -1){
  std::cerr << msg << std::endl;
  std::exit(exit_code);
}

template<typename T>
class matrix{
  T *arr = nullptr;
  
  std::vector<int> dim_sizes{};
  std::vector<int> dim_offsets{};
  int dims = -1;

  template<typename U, typename... Args>
  void construct_matrix(U dim_size, Args... args){
    dim_sizes.push_back(dim_size);

    construct_matrix(args...);
  }

  template<typename U>
  void construct_matrix(U dim_size){
    dim_sizes.push_back(dim_size);
    dims = dim_sizes.size();

    // need to reverse the sizes, since they are accessed when indexing into the array
    std::vector<int> reversed_sizes{};

    dim_offsets.resize(dims);
    dim_offsets[0] = 1;

    int cumulative_size = 1;
    for(int i = dims - 1; i >= 0; i--){
      cumulative_size *= dim_sizes[i];
      reversed_sizes.push_back(dim_sizes[i]);

      dim_offsets[dims-i] = cumulative_size;
    }
    
    dim_sizes = reversed_sizes;
    
    // cumulative_size is the total size at this point
    if(cumulative_size < 1) fatal_exit("Incorrect matrix size");

    arr = new T[cumulative_size];
  }

  template<typename U, typename... Args>
  T &get_item(int offset, int dim_num, U idx, Args... args){
    if(idx >= dim_sizes[dim_num]) fatal_exit("Out of bounds access");

    offset += dim_offsets[dim_num] * idx;

    return get_item(offset, dim_num - 1, args...);
  }

  template<typename U>
  T &get_item(int offset, int dim_num, U idx){
    if(idx >= dim_sizes[dim_num]) fatal_exit("Out of bounds access");

    offset += dim_offsets[dim_num] * idx;

    if(dim_num != 0) fatal_exit("Incorrect number of indexes");

    return arr[offset];
  }
public:
  template<typename U, typename... Args>
  matrix(U dim_size, Args... args){
    dim_sizes.push_back(dim_size);

    construct_matrix(args...);
  }

  matrix(int dim_size){
    dim_sizes.push_back(dim_size);
    dims = 1;
    
    if(dim_size < 1) fatal_exit("Incorrect matrix size");

    arr = new T[dim_size];
  }

  template<typename U, typename... Args>
  T &operator()(U idx, Args... args){
    int dim_num = dims - 1;

    if(idx >= dim_sizes[dim_num]) fatal_exit("Out of bounds access");

    int offset = dim_offsets[dim_num] * idx;

    return get_item(offset, dim_num - 1, args...);
  }

  T &operator()(int idx){
    if(idx >= dim_sizes[0]) fatal_exit("Out of bounds access");

    int dim_num = dims - 1;
    
    if(dim_num != 0) fatal_exit("Incorrect number of indexes");

    return arr[idx];
  }
  
  ~matrix(){
    delete arr;
  }
};

int main()
{
  matrix<int> mat{1, 2, 3, 4, 5, 6, 7};

  int count = 0;
  for(int i = 0; i < 1; i++){
    for(int j = 0; j < 2; j++){
      for(int k = 0; k < 3; k++){
        for(int l = 0; l < 4; l++){
          for(int m = 0; m < 5; m++){
            for(int n = 0; n < 6; n++){
              for(int o = 0; o < 7; o++){
                mat(i, j, k, l, m, n, o) = count++;
              }
            }
          }
        }
      }
    }
  }

  for(int i = 0; i < 1; i++){
    for(int j = 0; j < 2; j++){
      for(int k = 0; k < 3; k++){
        for(int l = 0; l < 4; l++){
          for(int m = 0; m < 5; m++){
            for(int n = 0; n < 6; n++){
              for(int o = 0; o < 7; o++){
                std::cout << mat(i, j, k, l, m, n, o) << "\n";
              }
            }
          }
        }
      }
    }
  }
  
  // ends up printing from 0 to 5039
}
