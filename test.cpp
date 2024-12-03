#include <iostream>
#include <chrono>
#include <optional>
#include <vector>
#include <thread>

#include "poly.h"

/*std::optional<double> poly_test(polynomial& p1,
                                polynomial& p2,
                                std::vector<std::pair<power, coeff>> solution)

{
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    polynomial p3 = p1 * p2;

    auto p3_can_form = p3.canonical_form();

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    p3.print();

    if (p3_can_form != solution)
    {
        return std::nullopt;
    }

    return std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
}*/

int main()
{
    //TEST MULTIPLICATION:
    //=============================================================================================================
    // /** We're doing (x+1)^2, so solution is x^2 + 2x + 1*/
    // std::vector<std::pair<power, coeff>> solution = {{2,1}, {1,2}, {0,1}};

    // /** This holds (x+1), which we'll pass to each polynomial */
    // std::vector<std::pair<power, coeff>> poly_input = {{1,1}, {0,1}};

    // polynomial p1(poly_input.begin(), poly_input.end());
    // polynomial p2(poly_input.begin(), poly_input.end());

    // std::optional<double> result = poly_test(p1, p2, solution);

    // if (result.has_value())
    // {
    //     std::cout << "Passed test, took " << result.value()/1000 << " seconds" << std::endl;
    // } 
    // else 
    // {
    //     std::cout << "Failed test" << std::endl;
    // }


    std::vector<std::pair<power, coeff>> input1 = {{2,2}, {1,2}, {0,2}, {7,0}};
    std::vector<std::pair<power, coeff>> input2 = {{4,6}, {2,6}, {1,6}, {0,6}};
    std::vector<std::pair<power, coeff>> input3 = {{9,10}, {6,6}, {4,4}, {0,2}};

    std::vector<std::pair<power, coeff>> inputSized1; //polynomial of [size1] terms of form  1+2x+3x^2+4^x3...
    std::vector<std::pair<power, coeff>> inputSized2; //polynomial of [size2] terms of form  3+6x+9x^2+12^x3...
    int size1 = 10000;
    int size2 = 10000;

    for(int i = 0; i< size1; i++){
        inputSized1.push_back(std::make_pair(i,(i+1)));
        //inputSized1.push_back(std::make_pair(6*i,(i+7) * (-1 * i)));
    }
    for(int i = 0; i< size2; i++){
        inputSized2.push_back(std::make_pair(i,3*(i+1)));
    }

    polynomial p1(input1.begin(), input1.end());
    polynomial p2(input2.begin(), input2.end());
    polynomial p3(input3.begin(), input3.end());

    polynomial ps1(inputSized1.begin(), inputSized1.end());
    polynomial ps2(inputSized2.begin(), inputSized2.end());

     unsigned int num_threads = std::thread::hardware_concurrency();
     std::cout << "Threads Avalible: " << num_threads << "\n";

    // std::vector<std::pair<power, coeff>> canon = p1.getPolyVec();

    // for(std::pair<power, coeff> pair : canon)
    // {
    //     std::cout << pair.first << ", " << pair.second << " | ";
    // }

    // //TEST SUMS:
    // //===================================================
    /* std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();//start clock to time adding 2 polys
     polynomial sum1 = ps1 + ps2;
     std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now(); //end clock
     auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
     std::cout << "SUM 1 COMPUTIATION TIME: " << duration.count() << " ms\n";

     begin = std::chrono::steady_clock::now();//start clock to time 
     std::vector<std::pair<power, coeff>> holdCanVec = sum1.canonical_form();
     end = std::chrono::steady_clock::now(); //end clock
     duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
     std::cout << "SUM 1 CANONIZATION TIME: " << duration.count() << " ms\n";

     //sum1.print();

     begin = std::chrono::steady_clock::now();//start clock to time adding   
     polynomial sum2 = ps1 + (int)5;
     end = std::chrono::steady_clock::now(); //end clock
     duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
     std::cout << "SUM 2 COMPUTIATION TIME: " << duration.count() << " ms\n";

     begin = std::chrono::steady_clock::now();//start clock to time 
     holdCanVec = sum2.canonical_form();
     end = std::chrono::steady_clock::now(); //end clock
     duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
     std::cout << "SUM 2 CANONIZATION TIME: " << duration.count() << " ms\n";
     //sum2.print();

     begin = std::chrono::steady_clock::now();//start clock to time adding   
     polynomial sum3 = (int)6 + ps1;
     end = std::chrono::steady_clock::now(); //end clock
     duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
     std::cout << "SUM 3 COMPUTIATION TIME: " << duration.count() << " ms\n";

     begin = std::chrono::steady_clock::now();//start clock to time 
     holdCanVec = sum3.canonical_form();
     end = std::chrono::steady_clock::now(); //end clock
     duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
     std::cout << "SUM 3 CANONIZATION TIME: " << duration.count() << " ms\n";*/
     //sum3.print();
     
     //non threaded timing fine i hope

    // //TEST MULTIPLY:
    // //===================================================

       std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();//start clock to time adding 2 polys
      polynomial mul1 = ps1 * ps2;
       std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now(); //end clock
       auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
       std::cout << "MUL 1 COMPUTIATION TIME: " << duration.count() << " ms\n";
       std::cout << "mul1 vector no canonization :\n";
       mul1.print();

       begin = std::chrono::steady_clock::now();//start clock to time 
       std::vector<std::pair<power, coeff>> holdCanVec = mul1.canonical_form();
       end = std::chrono::steady_clock::now(); //end clock
       duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
       std::cout << "MUL 1 CANONIZATION TIME: " << duration.count() << " ms\n";

     
    //   std::cout << "ps1 vector :\n";
    //   ps1.print();
    //        std::cout << "ps2 vector :\n";
    //   ps2.print();
    //    std::cout << "mul1 vector :\n";
    //      polynomial cans1(holdCanVec.begin(), holdCanVec.end());
    //   cans1.print();
    
     /*begin = std::chrono::steady_clock::now();//start clock to time adding   
     polynomial mul2 = ps1 * (int)5;
     end = std::chrono::steady_clock::now(); //end clock
     duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
     std::cout << "MUL 2 COMPUTIATION TIME: " << duration.count() << " ms\n";

     begin = std::chrono::steady_clock::now();//start clock to time 
     holdCanVec = mul2.canonical_form();
     end = std::chrono::steady_clock::now(); //end clock
     duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
     std::cout << "MUL 2 CANONIZATION TIME: " << duration.count() << " ms\n";
     //mul2.print(); */

     /*std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();//start clock to time adding   
     polynomial mul3 = (int)6 * ps1;
     std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now(); //end clock
     auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
     std::cout << "MUL 3 COMPUTIATION TIME: " << duration.count() << " ms\n";
        mul3.print();
     begin = std::chrono::steady_clock::now();//start clock to time 
     std::vector<std::pair<power, coeff>> holdCanVec = mul3.canonical_form();
     end = std::chrono::steady_clock::now(); //end clock
     duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
     std::cout << "MUL 3 CANONIZATION TIME: " << duration.count() << " ms\n";
     //mul3.print();
     std::cout << "mul3 vector :\n";
     polynomial cans1(holdCanVec.begin(), holdCanVec.end());
     cans1.print();*/
    
     //mul is pretty slow, but it is inherent without FFT that it is O(n^2)


    // polynomial mul1 = p1 * p2;
    // std::cout << "MUL 1: \n";
    // mul1.print();

    // polynomial mul2 = p1 * 5;
    // std::cout << "MUL 2: \n";
    // mul2.print();

    // polynomial mul3 = 6 * p1;
    // std::cout << "MUL 3: \n";
    // mul3.print();

    //TEST MODULUS:
    //===================================================
    
    //  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();//start clock to time adding 2 polys
    //  polynomial mod1 = p1 % p2;
    //  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now(); //end clock
    //  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    //  std::cout << "MOD 1 COMPUTIATION TIME: " << duration.count() << " ms\n";

    //  begin = std::chrono::steady_clock::now();//start clock to time 
    //  std::vector<std::pair<power, coeff>> holdCanVec = ps1.canonical_form();
    //  end = std::chrono::steady_clock::now(); //end clock
    //  duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    //  std::cout << "MOD 1 CANONIZATION TIME: " << duration.count() << " ms\n";

    //  begin = std::chrono::steady_clock::now();//start clock to time adding 2 polys
    //  polynomial mod2 = p2 % p1;
    //  end = std::chrono::steady_clock::now(); //end clock
    //  duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    //  std::cout << "MOD 2 COMPUTIATION TIME: " << duration.count() << " ms\n";

    //  begin = std::chrono::steady_clock::now();//start clock to time 
    //  holdCanVec = ps2.canonical_form();
    //  end = std::chrono::steady_clock::now(); //end clock
    //  duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    //  std::cout << "MOD 2 CANONIZATION TIME: " << duration.count() << " ms\n";

    // std::cout << "POLYS: \n";
    // // ps1.print();
    // // ps2.print();

    // std::cout << "MOD1: \n";
    // p1.print();
    // p2.print();
    // mod1.print();
    // std::cout << mod1.find_degree_of() << '\n';
    // std::cout << "MOD2: \n";
    // p3.print();
    // p2.print();
    // mod2.print();
    // std::cout << mod2.find_degree_of() << '\n';


    //TEST MODULUS:
    //===================================================
    //  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();//start clock to time adding 2 polys
    //  polynomial mul1 = ps1 % ps2;
    //  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now(); //end clock
    //  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    //  std::cout << "MOD 1 COMPUTIATION TIME: " << duration.count() << " ms\n";
    //  // std::cout << "mul1 vector no canonization :\n";
    //  //mul1.print();

    //  begin = std::chrono::steady_clock::now();//start clock to time 
    //  std::vector<std::pair<power, coeff>> holdCanVec = mul1.canonical_form();
    //  end = std::chrono::steady_clock::now(); //end clock
    //  duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    //  std::cout << "MOD 1 CANONIZATION TIME: " << duration.count() << " ms\n";

     
    //  std::cout << "ps1 vector :\n";
    //  ps1.print();
    //       std::cout << "ps2 vector :\n";
    //  ps2.print();
    //   std::cout << "mul1 vector :\n";
    //     polynomial cans1(holdCanVec.begin(), holdCanVec.end());
    //  cans1.print();




    //test SPECIAL SUB

    /*polynomial first = ps1.canonical_form();
    polynomial second = ps2.canonical_form();
    polynomial result = specialSub(second, first);
    second.print();
    first.print();
    result.print();*/
    
    





}