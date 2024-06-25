using InformationSystem.DAL.Enums;
using InformationSystem.DAL.Entities;
using Microsoft.EntityFrameworkCore;

namespace InformationSystem.DAL.Seeds;

public static class StudentSeeds
{
    public static readonly StudentEntity Michal = new()
    {
        Id = Guid.Parse("06a8a2cf-ea03-4095-a3e4-aa0291fe9c75"),
        Login = "xblaze38",
        Name = "Michal",
        Surname = "Blazek",
        PhotoUrl = null
    };

    public static readonly StudentEntity Tomas = new()
    {
        Id = Guid.Parse("df935095-8709-4040-a2bb-b6f97cb416dc"),
        Login = "xrezni33",
        Name = "Tomas",
        Surname = "Reznicek",
        PhotoUrl = null
    };

    public static readonly StudentEntity Matyas = new()
    {
        Id = Guid.Parse("918319c6-89c1-4829-b88d-66d2a868747b"),
        Login = "xsapik02",
        Name = "Matyas",
        Surname = "Sapik",
        PhotoUrl = new Uri("https://php.esports.cz/images/min.php?cover&exact&topcut&w=170&h=170&mtime=1646320275&file=https://www.hcdukla.cz//foto/hraci/mladez/2022/sapik_matyas.jpg")
    };

    public static readonly StudentEntity Krystof = new()
    {
        Id = Guid.Parse("2a8e60c7-35b0-4bce-9264-b303a8ed58df"),
        Login = "xmicha94",
        Name = "Krystof",
        Surname = "Michalek",
        PhotoUrl = null
    };

    public static readonly StudentEntity Matej = new()
    {
        Id = Guid.Parse("c3f5c001-9be6-4b36-b47f-ce0ad25f6a2d"),
        Login = "xlepes02",
        Name = "Matej",
        Surname = "Lepeska",
        PhotoUrl = null
    };

    static StudentSeeds()
    {
        Michal.Subjects.Add(StudentsInSubjectSeeds.MichalICS);
        Michal.ActivityEvaluation.Add(StudentEvaluationSeeds.MichalFinalExamICS);
        Michal.Subjects.Add(StudentsInSubjectSeeds.MichalIPK);
        Michal.Subjects.Add(StudentsInSubjectSeeds.MichalIPP);
        Michal.Subjects.Add(StudentsInSubjectSeeds.MichalIZU);
        Tomas.ActivityEvaluation.Add(StudentEvaluationSeeds.TomasMidtermICS);
        Tomas.Subjects.Add(StudentsInSubjectSeeds.TomasICS);
        Tomas.Subjects.Add(StudentsInSubjectSeeds.TomasIPP);
        Tomas.Subjects.Add(StudentsInSubjectSeeds.TomasIZU);
        Matyas.Subjects.Add(StudentsInSubjectSeeds.MatyasICS);
        Matej.Subjects.Add(StudentsInSubjectSeeds.MatejICS);
        Krystof.Subjects.Add(StudentsInSubjectSeeds.KrystofICS);
    }

    public static void Seed(this ModelBuilder modelBuilder) =>
        modelBuilder.Entity<StudentEntity>().HasData(
            Michal with { Subjects = Array.Empty<StudentsInSubjectEntity>(), ActivityEvaluation = Array.Empty<StudentEvaluationEntity>() },
            Tomas with { Subjects = Array.Empty<StudentsInSubjectEntity>(), ActivityEvaluation = Array.Empty<StudentEvaluationEntity>() },
            Matyas with { Subjects = Array.Empty<StudentsInSubjectEntity>(), ActivityEvaluation = Array.Empty<StudentEvaluationEntity>() },
            Krystof with { Subjects = Array.Empty<StudentsInSubjectEntity>(), ActivityEvaluation = Array.Empty<StudentEvaluationEntity>() },
            Matej with { Subjects = Array.Empty<StudentsInSubjectEntity>(), ActivityEvaluation = Array.Empty<StudentEvaluationEntity>()}
        );
}