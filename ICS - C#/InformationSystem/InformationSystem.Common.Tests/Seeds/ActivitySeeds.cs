using InformationSystem.DAL.Enums;
using InformationSystem.DAL.Entities;
using Microsoft.EntityFrameworkCore;

namespace InformationSystem.Common.Tests.Seeds;

public static class ActivitySeeds
{
    public static readonly ActivityEntity EmptyActivity = new()
    {
        Id = default,
        Start = default!,
        End = default!,
        Room = default!,
        ActivityType = default!,
        Description = default,
        SubjectId = default,
        Subject = default!
    };

    public static readonly ActivityEntity ExamActivityICS = new()
    {
        Id = Guid.Parse("a06d676e-38ff-4e1e-813d-6eeb83352de7"),
        Start = new DateTime(2024, 4, 1, 9, 0, 0),
        End = new DateTime(2024, 4, 1, 12, 0, 0),
        Room = Room.D105,
        ActivityType = ActivityType.FinalExam,
        Description = "Final Exam",
        SubjectId = SubjectSeeds.SubjectEntity1.Id,
        Subject = SubjectSeeds.SubjectEntity1
    };

    public static readonly ActivityEntity ActivityEntity1 = new()
    {
        Id = Guid.Parse("bd7d2f27-70a4-41b5-ba87-0b4a9a32406c"),
        Start = new DateTime(2024, 1, 1, 10, 30, 0),
        End = new DateTime(2024, 1, 1, 11, 0, 0),
        Room = Room.T69_420,
        ActivityType = ActivityType.Lab,
        Description = "Activity seeded activity 1 description",
        SubjectId = SubjectSeeds.SubjectEntity1.Id,
        Subject = SubjectSeeds.SubjectEntity1
    };

    public static readonly ActivityEntity ExamActivityICSUpdate = ExamActivityICS with { Id = Guid.Parse("e2fb35fc-4d9f-44c0-b645-8e4f95673838"), Subject = null!, SubjectId = SubjectSeeds.SubjectEntityUpdate.Id };
    public static readonly ActivityEntity ExamActivityICSDelete = ExamActivityICS with { Id = Guid.Parse("8ff02718-ef6b-4b59-9759-778d42b4d7d7"), Subject = null!, SubjectId = SubjectSeeds.SubjectEntityDelete.Id };

    public static readonly ActivityEntity ActivityEntityUpdate = ActivityEntity1 with { Id = Guid.Parse("50e134f1-f3d8-443b-8133-5f8e2d648482"), Subject = null!, SubjectId = SubjectSeeds.SubjectEntityUpdate.Id };
    public static readonly ActivityEntity ActivityEntityDelete = ActivityEntity1 with { Id = Guid.Parse("75d29d5c-9f7c-4dc3-93e3-7ff7ebfd8be1"), Subject = null!, SubjectId = SubjectSeeds.SubjectEntityDelete.Id };


    public static ActivityEntity ActivityEntity2 = new()
    {
        Id = Guid.Parse("db8f2f3c-cdb8-4e17-80b7-54f7614f5b5b"),
        Start = new DateTime(2022, 3, 1, 10, 0, 0),
        End = new DateTime(2022, 3, 2, 9, 0, 0),
        Room = Room.T10_210,
        ActivityType = ActivityType.Lecture,
        Description = "Activity seeded activity 2 description",
        SubjectId = SubjectSeeds.SubjectEntity1.Id,
        Subject = SubjectSeeds.SubjectEntity1
    };

    public static void Seed(this ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<ActivityEntity>().HasData(
            ActivityEntity1 with { Subject = null! },
            ActivityEntity2 with { Subject = null! },
            ExamActivityICS with { Subject = null! },
            ExamActivityICSUpdate,
            ExamActivityICSDelete,
            ActivityEntityUpdate,
            ActivityEntityDelete
        );
    }
}
