#include <seal/seal.h>
#include <chrono>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;
using namespace seal;

void log_result(ofstream &log_file, const string &operation, size_t poly_degree, size_t vector_size, double time_ms) {
    log_file << operation << "," << poly_degree << "," << vector_size << "," << time_ms << "\n";
}

int main() {
    vector<size_t> poly_degrees = {1024, 2048, 4096, 8192, 16384};
    vector<size_t> vector_sizes = {1000, 10000, 100000, 1000000};

    ofstream log_file("seal_benchmark_log.csv", ios::app);
    log_file << "Operation,PolyModulusDegree,VectorSize,Time(ms)\n";

    for (size_t poly_deg : poly_degrees) {
        EncryptionParameters parms(scheme_type::bfv);
        parms.set_poly_modulus_degree(poly_deg);
        parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_deg));
        parms.set_plain_modulus(PlainModulus::Batching(poly_deg, 20));

        SEALContext context(parms);
        KeyGenerator keygen(context);

        PublicKey public_key;
        keygen.create_public_key(public_key);
        SecretKey secret_key = keygen.secret_key();

        Encryptor encryptor(context, public_key);
        Evaluator evaluator(context);
        Decryptor decryptor(context, secret_key);
        BatchEncoder encoder(context);

        for (size_t vec_size : vector_sizes) {
            size_t slot_count = encoder.slot_count();
            if (vec_size > slot_count) continue;

            vector<uint64_t> vec1(vec_size, 3);
            vector<uint64_t> vec2(vec_size, 5);
            Plaintext plain1, plain2;
            encoder.encode(vec1, plain1);
            encoder.encode(vec2, plain2);

            Ciphertext enc1, enc2;
            encryptor.encrypt(plain1, enc1);
            encryptor.encrypt(plain2, enc2);

            // ----- Cipher + Cipher -----
            auto start = chrono::high_resolution_clock::now();
            Ciphertext result_add;
            evaluator.add(enc1, enc2, result_add);
            auto end = chrono::high_resolution_clock::now();
            log_result(log_file, "Cipher+Cipher_Add", poly_deg, vec_size,
                       chrono::duration<double, std::milli>(end - start).count());

            start = chrono::high_resolution_clock::now();
            Ciphertext result_mul;
            evaluator.multiply(enc1, enc2, result_mul);
            end = chrono::high_resolution_clock::now();
            log_result(log_file, "Cipher+Cipher_Mul", poly_deg, vec_size,
                       chrono::duration<double, std::milli>(end - start).count());

            // ----- Cipher + Plain -----
            start = chrono::high_resolution_clock::now();
            Ciphertext result_pc_add;
            evaluator.add_plain(enc1, plain2, result_pc_add);
            end = chrono::high_resolution_clock::now();
            log_result(log_file, "Cipher+Plain_Add", poly_deg, vec_size,
                       chrono::duration<double, std::milli>(end - start).count());

            start = chrono::high_resolution_clock::now();
            Ciphertext result_pc_mul;
            evaluator.multiply_plain(enc1, plain2, result_pc_mul);
            end = chrono::high_resolution_clock::now();
            log_result(log_file, "Cipher+Plain_Mul", poly_deg, vec_size,
                       chrono::duration<double, std::milli>(end - start).count());

            // ----- Plain + Cipher (reversed) -----
            start = chrono::high_resolution_clock::now();
            Ciphertext result_cp_add;
            evaluator.add_plain(enc2, plain1, result_cp_add);
            end = chrono::high_resolution_clock::now();
            log_result(log_file, "Plain+Cipher_Add", poly_deg, vec_size,
                       chrono::duration<double, std::milli>(end - start).count());

            start = chrono::high_resolution_clock::now();
            Ciphertext result_cp_mul;
            evaluator.multiply_plain(enc2, plain1, result_cp_mul);
            end = chrono::high_resolution_clock::now();
            log_result(log_file, "Plain+Cipher_Mul", poly_deg, vec_size,
                       chrono::duration<double, std::milli>(end - start).count());

            cout << "✅ Done: Degree=" << poly_deg << ", VectorSize=" << vec_size << endl;
        }
    }

    log_file.close();
    cout << "\n🎉 Benchmarking complete. Check seal_benchmark_log.csv\n";
    return 0;
}

