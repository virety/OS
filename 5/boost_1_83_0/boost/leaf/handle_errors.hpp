#ifndef BOOST_LEAF_HANDLE_ERRORS_HPP_INCLUDED
#define BOOST_LEAF_HANDLE_ERRORS_HPP_INCLUDED

// Copyright 2018-2022 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/config.hpp>
#include <boost/leaf/context.hpp>
#include <boost/leaf/capture.hpp>
#include <boost/leaf/detail/demangle.hpp>

namespace boost { namespace leaf {

class BOOST_LEAF_SYMBOL_VISIBLE error_info
{
    error_info & operator=( error_info const & ) = delete;

#ifndef BOOST_LEAF_NO_EXCEPTIONS
    static error_id unpack_error_id( std::exception const * ex ) noexcept
    {
#if BOOST_LEAF_CFG_STD_SYSTEM_ERROR
        if( std::system_error const * se = dynamic_cast<std::system_error const *>(ex) )
            if( is_error_id(se->code()) )
                return leaf_detail::make_error_id(se->code().value());
        if( std::error_code const * ec = dynamic_cast<std::error_code const *>(ex) )
            if( is_error_id(*ec) )
                return leaf_detail::make_error_id(ec->value());
#endif
        if( error_id const * err_id = dynamic_cast<error_id const *>(ex) )
            return *err_id;
        return current_error();
    }

    std::exception * const ex_;
#endif

    error_id const err_id_;

protected:

    error_info( error_info const & ) noexcept = default;

    template <class CharT, class Traits>
    void print( std::basic_ostream<CharT, Traits> & os ) const
    {
        os << "Error ID = " << err_id_.value();
#ifndef BOOST_LEAF_NO_EXCEPTIONS
        if( ex_ )
        {
            os <<
                "\nException dynamic type: " << leaf_detail::demangle(typeid(*ex_).name()) <<
                "\nstd::exception::what(): " << ex_->what();
        }
#endif
    }

public:

    BOOST_LEAF_CONSTEXPR explicit error_info( error_id id ) noexcept:
#ifndef BOOST_LEAF_NO_EXCEPTIONS
        ex_(nullptr),
#endif
        err_id_(id)
    {
    }

#ifndef BOOST_LEAF_NO_EXCEPTIONS
    explicit error_info( std::exception * ex ) noexcept:
        ex_(ex),
        err_id_(unpack_error_id(ex_))
    {
    }
#endif

    BOOST_LEAF_CONSTEXPR error_id error() const noexcept
    {
        return err_id_;
    }

    BOOST_LEAF_CONSTEXPR std::exception * exception() const noexcept
    {
#ifdef BOOST_LEAF_NO_EXCEPTIONS
        return nullptr;
#else
        return ex_;
#endif
    }

    template <class CharT, class Traits>
    friend std::basic_ostream<CharT, Traits> & operator<<( std::basic_ostream<CharT, Traits> & os, error_info const & x )
    {
        os << "leaf::error_info: ";
        x.print(os);
        return os << '\n';
    }
};

////////////////////////////////////////

#if BOOST_LEAF_CFG_DIAGNOSTICS

class BOOST_LEAF_SYMBOL_VISIBLE diagnostic_info: public error_info
{
    leaf_detail::e_unexpected_count const * e_uc_;
    void const * tup_;
    void (*print_)( std::ostream &, void const * tup, int key_to_print );

protected:

    diagnostic_info( diagnostic_info const & ) noexcept = default;

    template <class Tup>
    BOOST_LEAF_CONSTEXPR diagnostic_info( error_info const & ei, leaf_detail::e_unexpected_count const * e_uc, Tup const & tup ) noexcept:
        error_info(ei),
        e_uc_(e_uc),
        tup_(&tup),
        print_(&leaf_detail::tuple_for_each<std::tuple_size<Tup>::value, Tup>::print)
    {
    }

public:

    template <class CharT, class Traits>
    friend std::basic_ostream<CharT, Traits> & operator<<( std::basic_ostream<CharT, Traits> & os, diagnostic_info const & x )
    {
        os << "leaf::diagnostic_info for ";
        x.print(os);
        os << ":\n";
        x.print_(os, x.tup_, x.error().value());
        if( x.e_uc_  )
            x.e_uc_->print(os);
        return os;
    }
};

namespace leaf_detail
{
    struct diagnostic_info_: diagnostic_info
    {
        template <class Tup>
        BOOST_LEAF_CONSTEXPR diagnostic_info_( error_info const & ei, leaf_detail::e_unexpected_count const * e_uc, Tup const & tup ) noexcept:
            diagnostic_info(ei, e_uc, tup)