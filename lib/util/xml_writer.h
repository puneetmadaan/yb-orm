#ifndef YB__UTIL__XML_WRITER__INCLUDED
#define YB__UTIL__XML_WRITER__INCLUDED

#include <string>
#include <boost/utility.hpp>
#include <boost/lexical_cast.hpp>
#include "UnicodeSupport.h"

struct _xmlBuffer;
typedef struct _xmlBuffer xmlBuffer;
typedef xmlBuffer *xmlBufferPtr;

struct _xmlTextWriter;
typedef struct _xmlTextWriter xmlTextWriter;
typedef xmlTextWriter *xmlTextWriterPtr;

namespace Yb {

namespace Writer {

// Not thread-safe

class Document: private boost::noncopyable
{
    friend class Element;

    xmlBufferPtr buffer_;
    xmlTextWriterPtr writer_;
    std::string xml_;

private:
    void write_raw (std::string const & value);

    void write_attribute (Yb::String const & name, Yb::String const & value);

    void write_string (Yb::String const & value);

    template <class TValue>
    void write_attribute (Yb::String const & name, TValue const & value)
    {
        write_attribute (name, boost::lexical_cast<Yb::String> (value));
    }

    template <class TValue>
    void write_string (TValue const & value)
    {
        write_string (boost::lexical_cast<Yb::String> (value));
    }

    void start_element (Yb::String const & name);

    void end_element ();

public:
    Document (std::string const & xml = "");
    ~Document ();

    void flush () const;

    // insert content
    void insert (Document const & doc);

    std::string const & end_document ();

    std::string const get_string () const;

    xmlTextWriterPtr get_writer ();
};

class Element: private boost::noncopyable
{
    Document & doc_;
    Yb::String name_;
    bool closed_;

    void start_element ();

    void close_element ();

public:
    Element (Document & doc, Yb::String const & name);

    template <class TValue>
    Element (Document & doc, Yb::String const & name, TValue const & value)
        :doc_ (doc)
        ,name_ (name)
        ,closed_ (false)
    {
        start_element ();
        doc_.write_string (value);
    }

    ~Element ();

    void set_content (Yb::String const & content);

    template <class TValue>
    void add_attribute (Yb::String const & name, TValue const & value)
    {
        doc_.write_attribute (name, value);
    }

    template <class TValue>
    void add_element (Yb::String const & name, TValue const & value)
    {
        Element (doc_, name, value);
    }
};

class Attribute: private boost::noncopyable
{
public:
    template <class TValue>
    Attribute (Element &element, Yb::String const & name, TValue const & value)
    {
        element.add_attribute (name, value);
    }

    ~Attribute ();
};

} // end of namespace Writer

} // end of namespace Yb

// vim:ts=4:sts=4:sw=4:et:
#endif // YB__UTIL__XML_WRITER__INCLUDED
