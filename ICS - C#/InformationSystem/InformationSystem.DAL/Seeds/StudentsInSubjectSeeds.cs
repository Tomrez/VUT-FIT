using InformationSystem.DAL.Enums;
using InformationSystem.DAL.Entities;
using Microsoft.EntityFrameworkCore;

namespace InformationSystem.DAL.Seeds;

public static class StudentsInSubjectSeeds
{
    public static readonly StudentsInSubjectEntity MichalICS = new()
    {
        Id = Guid.Parse("0d4fa150-ad80-4d46-a511-4c666166ec5e"),
        SubjectId = SubjectSeeds.ICSSubject.Id,
        StudentId = StudentSeeds.Michal.Id,
        Subject = null!,
        Student = null!
    };

    public static readonly StudentsInSubjectEntity MichalIPP = new()
    {
        Id = Guid.Parse("bed99195-250b-43bd-aaf0-764cc024d02f"),
        SubjectId = SubjectSeeds.IPPSubject.Id,
        StudentId = StudentSeeds.Michal.Id,
        Subject = null!,
        Student = null!
    };

    public static readonly StudentsInSubjectEntity MichalIZU = new()
    {
        Id = Guid.Parse("08cb9cd2-4513-48f9-b3c8-55a00d68f4f8"),
        SubjectId = SubjectSeeds.IZUSubject.Id,
        StudentId = StudentSeeds.Michal.Id,
        Subject = null!,
        Student = null!
    };

    public static readonly StudentsInSubjectEntity MichalIPK = new()
    {
        Id = Guid.Parse("f3a44fc7-7b6b-43e0-ae20-8a899af96ab3"),
        SubjectId = SubjectSeeds.IPKSubject.Id,
        StudentId = StudentSeeds.Michal.Id,
        Subject = null!,
        Student = null!
    };

    public static readonly StudentsInSubjectEntity TomasICS = new()
    {
        Id = Guid.Parse("87833e66-05ba-4d6b-900b-fe5ace88dbd8"),
        SubjectId = SubjectSeeds.ICSSubject.Id,
        StudentId = StudentSeeds.Tomas.Id,
        Subject = null!,
        Student = null!
    };

    public static readonly StudentsInSubjectEntity TomasIPP = new()
    {
        Id = Guid.Parse("ec04aa01-1bd2-4fd1-b005-adb5cddfa642"),
        SubjectId = SubjectSeeds.IPPSubject.Id,
        StudentId = StudentSeeds.Tomas.Id,
        Subject = null!,
        Student = null!
    };

    public static readonly StudentsInSubjectEntity TomasIZU = new()
    {
        Id = Guid.Parse("13dd0ba3-d9ff-4b42-a0ae-7bc0e04df0e4"),
        SubjectId = SubjectSeeds.IZUSubject.Id,
        StudentId = StudentSeeds.Tomas.Id,
        Subject = null!,
        Student = null!
    };

    public static readonly StudentsInSubjectEntity MatyasICS = new()
    {
        Id = Guid.Parse("5d7eab54-d023-4b30-8b52-960a632e19a1"),
        SubjectId = SubjectSeeds.ICSSubject.Id,
        StudentId = StudentSeeds.Matyas.Id,
        Subject = null!,
        Student = null!
    };

    public static readonly StudentsInSubjectEntity KrystofICS = new()
    {
        Id = Guid.Parse("714e665b-cbd1-4d15-b951-b0c24070e55b"),
        SubjectId = SubjectSeeds.ICSSubject.Id,
        StudentId = StudentSeeds.Krystof.Id,
        Subject = null!,
        Student = null!
    };

    public static readonly StudentsInSubjectEntity MatejICS = new()
    {
        Id = Guid.Parse("484218be-abc3-480d-be5f-bb8f5c3c7e6e"),
        SubjectId = SubjectSeeds.ICSSubject.Id,
        StudentId = StudentSeeds.Matej.Id,
        Subject = null!,
        Student = null!
    };

    public static void Seed(this ModelBuilder modelBuilder) =>
        modelBuilder.Entity<StudentsInSubjectEntity>().HasData(
            MichalICS,
            MichalIPK,
            MichalIPP,
            MichalIZU,
            TomasICS,
            TomasIPP,
            TomasIZU,
            MatyasICS,
            MatejICS,
            KrystofICS
        );
}