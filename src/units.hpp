#ifndef UNITS_HPP
#define UNITS_HPP

class text_space_units {
public:
    text_space_units() noexcept {}

    static text_space_units from_units(double units) noexcept {
        return text_space_units(units);
    }

    static text_space_units from_inches(double inches) noexcept {
        return text_space_units(inches * 72.0);
    }

    double to_double() noexcept {
        return units;
    }

private:
    text_space_units(double units) noexcept : units(units) {}

    double units = 0.0; // unscaled text-space units
};

inline std::ostream& operator<<(std::ostream& os, text_space_units units) {
    os << units.to_double() << "tsu";
    return os;
}

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
    percentage(double percent) noexcept : percent(percent) {}

    double percent = 0.0;
};

inline std::ostream& operator<<(std::ostream& os, percentage percent) {
    os << percent.to_double() << '%';
    return os;
}

#endif