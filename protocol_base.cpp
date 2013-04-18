/**
 *
 *
 **/
#include "protocol_base.hpp"
#include <iostream>

using std::wcout;

/**
 *
 *
 **/
protocol_base_t::protocol_base_t() {
    wcout << L"[" << this << L"][protocol_base_t][constructor]\n";
}

/**
 *
 *
 **/
protocol_base_t::~protocol_base_t() {
    wcout << L"[" << this << L"][protocol_base_t][destructor]\n";
}

/**
 *
 *
 **/
void protocol_base_t::init() {
    wcout << L"[" << this << L"][protocol_base_t][init]\n";
    init_impl();
}

/**
 *
 *
 **/
void protocol_base_t::connect() {
    wcout << L"[" << this << L"][protocol_base_t][connect]\n";
    connect_impl();
}

/**
 *
 *
 **/
void protocol_base_t::recive() {
    wcout << L"[" << this << L"][protocol_base_t][recive]\n";
    recive_impl();
}
