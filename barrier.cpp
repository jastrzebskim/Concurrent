#include <iostream>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <chrono>
#include <array>
#include <ctime>

class Barrier {
private:
    int resistance;
    std::condition_variable cv;
    std::mutex cv_m;
public:
    Barrier(int res) {
        resistance = res;
    }

    void reach() {
        std::unique_lock<std::mutex> lk(cv_m);
        if (resistance == 0) {
            lk.unlock();
        }
        else if (resistance == 1) {
            resistance = 0;
            lk.unlock();
            cv.notify_all();
        }
        else if (resistance > 1) {
            resistance--;
            cv.wait(lk, [&] {return resistance == 0;});
        }
    }
};

void f1(Barrier *b) {
    printf("I started\n");
    b->reach();
    std::cout << "Message1\n";
}

void f2(Barrier *b) {
    std::cout << "hi\n";
    clock_t poczatek, koniec;
    double roz;
    b->reach();
    std::cout << "Message2\n";
}

void f3(Barrier *b) {
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    b->reach();
    std::cout << "Message3\n";
}
int main() {
    Barrier *b = new Barrier(3);
    std::array<std::thread, 5> threads = {std::thread{[b]{f1(b);}}, std::thread{[b]{f2(b);}},
                                          std::thread{[b]{f3(b);}}, std::thread{[b]{f1(b);}},
                                          std::thread{[b]{f1(b);}}};
    for (auto& t : threads)
        t.join();
    return 0;
}
