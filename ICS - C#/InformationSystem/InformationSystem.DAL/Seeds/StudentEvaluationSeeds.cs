using InformationSystem.DAL.Enums;
using InformationSystem.DAL.Entities;
using Microsoft.EntityFrameworkCore;

namespace InformationSystem.DAL.Seeds;

public static class StudentEvaluationSeeds
{
    public static readonly StudentEvaluationEntity MichalFinalExamICS = new()
    {
        Id = Guid.Parse("f2144f3f-8907-4791-b060-bcf516306d16"),
        StudentId = StudentSeeds.Michal.Id,
        ActivityId = ActivitySeeds.ExamActivityICS.Id,
        Points = 55,
        Notes = "Well done!",
        Evaluator = null,

        Student = StudentSeeds.Michal,
        Activity = ActivitySeeds.ExamActivityICS
    };

    public static readonly StudentEvaluationEntity TomasMidtermICS = new()
    {
        Id = Guid.Parse("d5822814-79ff-4124-bd01-269aec3a94d7"),
        StudentId = StudentSeeds.Tomas.Id,
        ActivityId = ActivitySeeds.MidtermActivityICS.Id,
        Points = 2,
        Notes = null,
        Evaluator = null,

        Student = StudentSeeds.Tomas,
        Activity = ActivitySeeds.MidtermActivityICS
    };

    public static void Seed(this ModelBuilder modelBuilder) =>
        modelBuilder.Entity<StudentsInSubjectEntity>().HasData(
            MichalFinalExamICS with { Student = null!, Activity = null! },
            TomasMidtermICS with { Student = null!, Activity = null! }
        );
}