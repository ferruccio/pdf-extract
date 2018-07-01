#ifndef UNITS_HPP
#define UNITS_HPP

// text space units
class text_space_units {
public:
    text_space_units() noexcept {}

    static text_space_units from_units(double units) noexcept {
        return text_space_units(units);
    }

    static text_space_units from_inches(double inches) noexcept {
        return from_units(inches * 72.0);
    }

    double to_double() noexcept {
        return units;
    }

private:
    explicit text_space_units(double units) noexcept : units(units) {}

    double units = 0.0; // unscaled text-space units
};

inline std::ostream& operator<<(std::ostream& os, text_space_units units) {
    os << units.to_double() << "u";
    return os;
}

// text space milliunits
class text_space_milliunits {
public:
    text_space_milliunits() {}

    static text_space_milliunits from_milliunits(double milliunits) noexcept {
        return text_space_milliunits(milliunits);
    }

    static text_space_milliunits from_units(text_space_units units) noexcept {
        return from_milliunits(units.to_double() * 1000.0);
    }

    double to_double() noexcept {
        return milliunits;
    }

private:
    explicit text_space_milliunits(double milliunits) noexcept : milliunits(milliunits) {}
    double milliunits = 0.0; // 1/1000ths of a text-space unit
};

inline std::ostream& operator<<(std::ostream& os, text_space_milliunits milliunits) {
    os << milliunits.to_double() << "mu";
    return os;
}

// percentage
class percentage {
public:
    percentage() noexcept {}

    static percentage from_double(double percent) noexcept {
        return percentage(percent);
    }

    double to_double() noexcept {
        return percent;
    }

    double percent_of(double source) noexcept {
        return source * (percent / 100.0);
    }

private:
    explicit percentage(double percent) noexcept : percent(percent) {}

    double percent = 0.0;
};

inline std::ostream& operator<<(std::ostream& os, percentage percent) {
    os << percent.to_double() << '%';
    return os;
}

#endif