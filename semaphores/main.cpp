#include <iostream>
#include <thread>
#include <semaphore.h>
#include <vector>
#include <chrono>
#include <atomic>

std::binary_semaphore sem_supplier(1); // Семафор для управления доступом поставщиков

std::binary_semaphore sem_rose(0), sem_violet(0), sem_peony(0);

std::vector<int> roses;
std::vector<int> violets;
std::vector<int> peonies;

void Supplier(int id)
{
    while (true)
    {
        if (id == 1)
        {
            sem_supplier.acquire();

            std::this_thread::sleep_for(std::chrono::seconds(1)); // Имитация времени поставки
            roses.push_back(1);
            violets.push_back(2);
            sem_rose.release();
            sem_violet.release();

            std::time_t now = std::time(nullptr);
            std::tm *local_time = std::localtime(&now);

            std::cout << "Поставщик V1 поставляет розы и фиалки. " << local_time->tm_hour << ':' << local_time->tm_min << ':' << local_time->tm_sec << ".\n";
        }
        else if (id == 2)
        {
            sem_supplier.acquire();

            std::this_thread::sleep_for(std::chrono::seconds(1)); // Имитация времени поставки
            peonies.push_back(3);
            roses.push_back(1);
            sem_rose.release();
            sem_peony.release();

            std::time_t now = std::time(nullptr);
            std::tm *local_time = std::localtime(&now);

            std::cout << "Поставщик V2 поставляет пионы и розы. " << local_time->tm_hour << ':' << local_time->tm_min << ':' << local_time->tm_sec << ".\n";
        }
        else if (id == 3)
        {
            sem_supplier.acquire();

            std::this_thread::sleep_for(std::chrono::seconds(1)); // Имитация времени поставки
            peonies.push_back(3);
            violets.push_back(2);
            sem_rose.release();
            sem_violet.release();

            std::time_t now = std::time(nullptr);
            std::tm *local_time = std::localtime(&now);

            std::cout << "Поставщик V3 поставляет пионы и фиалки. " << local_time->tm_hour << ':' << local_time->tm_min << ':' << local_time->tm_sec << ".\n";
        }
    }
}

void BouquetMaker(int id)
{
    while (true)
    {
        std::time_t now = std::time(nullptr);
        std::tm *local_time = std::localtime(&now);
        if (id == 1)
        {
            if (roses.size() != 0 && violets.size() != 0)
            {
                sem_rose.acquire();
                sem_violet.acquire();

                std::cout << "Составитель S1 делает букет с розами и фиалками. " << local_time->tm_hour << ':' << local_time->tm_min << ':' << local_time->tm_sec << ".\n";

                roses.erase(roses.begin());
                violets.erase(violets.begin());
            }
            else
            {
                std::cout << "Для составителя S1 не хватает цветов. " << local_time->tm_hour << ':' << local_time->tm_min << ':' << local_time->tm_sec << ".\n";
            }

            std::this_thread::sleep_for(std::chrono::seconds(2)); 
            sem_supplier.release();
        }
        else if (id == 2)
        {
            if (roses.size() != 0 && peonies.size() != 0)
            {
                sem_rose.acquire();
                sem_peony.acquire();

                std::time_t now = std::time(nullptr);
                std::tm *local_time = std::localtime(&now);

                std::cout << "Составитель S2 делает букет с розами и пионами. " << local_time->tm_hour << ':' << local_time->tm_min << ':' << local_time->tm_sec << ".\n";
                roses.erase(roses.begin());
                peonies.erase(peonies.begin());
            }
            else
            {
                std::cout << "Для составителя S2 не хватает цветов. " << local_time->tm_hour << ':' << local_time->tm_min << ':' << local_time->tm_sec << ".\n";
            }
            std::this_thread::sleep_for(std::chrono::seconds(2)); 
            sem_supplier.release();
        }
        else if (id == 3)
        {
            if (peonies.size() != 0 && violets.size() != 0)
            {
                sem_peony.acquire();
                sem_violet.acquire();

                std::time_t now = std::time(nullptr);
                std::tm *local_time = std::localtime(&now);

                std::cout << "Составитель S3 делает букет с пионами и фиалками. " << local_time->tm_hour << ':' << local_time->tm_min << ':' << local_time->tm_sec << ".\n";
                peonies.erase(peonies.begin());
                violets.erase(violets.begin());
            }
            else
            {
                std::cout << "Для составителя S3 не хватает цветов. " << local_time->tm_hour << ':' << local_time->tm_min << ':' << local_time->tm_sec << ".\n";
            }

            std::this_thread::sleep_for(std::chrono::seconds(2)); 
            sem_supplier.release();
        }
    }
}

int main()
{
    std::thread suppliers[] = {std::thread(Supplier, 1), std::thread(Supplier, 2), std::thread(Supplier, 3)};
    std::thread bouquet_makers[] = {std::thread(BouquetMaker, 1), std::thread(BouquetMaker, 2), std::thread(BouquetMaker, 3)};

    for (auto &supplier : suppliers)
        supplier.join();
    for (auto &bouquet_maker : bouquet_makers)
        bouquet_maker.join();

    return 0;
}
