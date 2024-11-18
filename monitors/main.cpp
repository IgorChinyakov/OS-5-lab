#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>
#include <condition_variable>

std::mutex monitor;
std::condition_variable cv_supplier, cv_bouquet;

std::vector<int> roses;
std::vector<int> violets;
std::vector<int> peonies;

bool supplier_active = true; // Флаг для управления доступом поставщиков

void Supplier(int id)
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(monitor);
        cv_supplier.wait(lock, [] { return supplier_active; }); // Ждать разрешения на поставку

        std::this_thread::sleep_for(std::chrono::seconds(1)); // Имитация времени поставки
        if (id == 1)
        {
            roses.push_back(1);
            violets.push_back(2);
            std::time_t now = std::time(nullptr);
            std::tm *local_time = std::localtime(&now);
            std::cout << "Поставщик V1 поставляет розы и фиалки. " << local_time->tm_hour << ':' << local_time->tm_min << ':' << local_time->tm_sec << ".\n";
        }
        else if (id == 2)
        {
            peonies.push_back(3);
            roses.push_back(1);

            std::time_t now = std::time(nullptr);
            std::tm *local_time = std::localtime(&now);
            std::cout << "Поставщик V2 поставляет пионы и розы. " << local_time->tm_hour << ':' << local_time->tm_min << ':' << local_time->tm_sec << ".\n";
        }
        else if (id == 3)
        {
            peonies.push_back(3);
            violets.push_back(2);

            std::time_t now = std::time(nullptr);
            std::tm *local_time = std::localtime(&now);
            std::cout << "Поставщик V3 поставляет пионы и фиалки. " << local_time->tm_hour << ':' << local_time->tm_min << ':' << local_time->tm_sec << ".\n";
        }

        supplier_active = false;
        cv_bouquet.notify_all(); // Разрешить составителям работать
    }
}

void BouquetMaker(int id)
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(monitor);
        cv_bouquet.wait(lock, [] { return !supplier_active; }); // Ждать, пока поставщики не завершат работу

        bool bouquet_made = false;
        if (id == 1 && roses.size() > 0 && violets.size() > 0)
        {
            roses.pop_back();
            violets.pop_back();

            std::time_t now = std::time(nullptr);
            std::tm *local_time = std::localtime(&now);
            std::cout << "Составитель S1 делает букет с розами и фиалками. " << local_time->tm_hour << ':' << local_time->tm_min << ':' << local_time->tm_sec << ".\n";
            bouquet_made = true;
        }
        else if (id == 2 && roses.size() > 0 && peonies.size() > 0)
        {
            roses.pop_back();
            peonies.pop_back();

            std::time_t now = std::time(nullptr);
            std::tm *local_time = std::localtime(&now);
            std::cout << "Составитель S2 делает букет с розами и пионами. " << local_time->tm_hour << ':' << local_time->tm_min << ':' << local_time->tm_sec << ".\n";
            bouquet_made = true;
        }
        else if (id == 3 && peonies.size() > 0 && violets.size() > 0)
        {
            peonies.pop_back();
            violets.pop_back();

            std::time_t now = std::time(nullptr);
            std::tm *local_time = std::localtime(&now);
            std::cout << "Составитель S3 делает букет с пионами и фиалками. " << local_time->tm_hour << ':' << local_time->tm_min << ':' << local_time->tm_sec << ".\n";
            bouquet_made = true;
        }

        if (!bouquet_made)
        {
            std::time_t now = std::time(nullptr);
            std::tm *local_time = std::localtime(&now);
            std::cout << "Для составителя S" << id << " не хватает цветов. " << local_time->tm_hour << ':' << local_time->tm_min << ':' << local_time->tm_sec << ".\n";
        }

        std::this_thread::sleep_for(std::chrono::seconds(1)); // Имитация времени сборки букета
        supplier_active = true;
        cv_supplier.notify_one(); // Разрешить поставщикам работать
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
