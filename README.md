# University Course Registration, Timetable & Attendance Management System

## About the Project

This project is a C++ based University Course Registration, Timetable and Attendance Management System.

The system is designed to help a university manage students, lecturers, administrators, courses, course registration, timetables and attendance.

The main purpose of the system is to replace some of the manual work involved in managing course enrolments and attendance with a simple object-oriented application.

## Main Features

### Student

Students can:

- Log into the system
- View available courses
- Register for courses
- Drop registered courses
- View their timetable
- View attendance information
- Check their registered courses

The system checks course capacity and prerequisites when a student tries to register.

### Lecturer

Lecturers can:

- Log into the system
- View the courses assigned to them
- View enrolled students
- Open attendance sessions
- Mark student attendance
- Close attendance sessions
- View attendance information

### Administrator

Administrators can:

- Add new users
- Update users
- Manage courses
- Add new course offerings
- Edit course information
- Remove courses
- Generate system reports

When an administrator creates a new course, weekly timetable slots can also be added. These slots are then shown in the student's timetable after the student registers for the course.

## Course Types

The system supports different types of courses using inheritance.

- Lecture Based Course
- Lab Based Course
- Project Based Course

Each course type can have its own course-related behaviour while sharing the common properties of the base `Course` class.

## Timetable

Each course can have one or more weekly time slots.

A time slot contains:

- Day
- Starting time
- Ending time
- Location

The system checks for timetable clashes when courses are registered.
