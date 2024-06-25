using InformationSystem.DAL.Enums;
using InformationSystem.DAL.Entities;
using Microsoft.EntityFrameworkCore;

namespace InformationSystem.DAL.Seeds;

public static class ActivitySeeds
{
    public static readonly ActivityEntity ExamActivityICS = new()
    {
        Id = Guid.Parse("0caf96a2-8f27-455f-985f-d65670685e59"),
        Start = new DateTime(2024, 4, 1, 9, 0, 0),
        End = new DateTime(2024, 4, 1, 12, 0, 0),
        Room = Room.D105,
        ActivityType = ActivityType.FinalExam,
        Description = "Final Exam",
        SubjectId = SubjectSeeds.ICSSubject.Id,
        Subject = SubjectSeeds.ICSSubject
    };

    public static readonly ActivityEntity MidtermActivityICS = new()
    {
        Id = Guid.Parse("4ac4954e-dead-46ee-9cc3-8164d0248797"),
        Start = new DateTime(2024, 3, 11, 8, 30, 0),
        End = new DateTime(2024, 3, 11, 9, 0, 0),
        Room = Room.T10_005,
        ActivityType = ActivityType.Midterm,
        Description = null,
        SubjectId = SubjectSeeds.ICSSubject.Id,
        Subject = SubjectSeeds.ICSSubject
    };

    public static readonly ActivityEntity LectureActivityICS = new()
    {
        Id = Guid.Parse("c8e052f7-d52b-4244-8236-0ad07646ca2f"),
        Start = new DateTime(2024, 3, 7, 8, 30, 0),
        End = new DateTime(2024, 3, 8, 9, 0, 0),
        Room = Room.T10_005,
        ActivityType = ActivityType.Lecture,
        Description = null,
        SubjectId = SubjectSeeds.ICSSubject.Id,
        Subject = SubjectSeeds.ICSSubject
    };

    public static readonly ActivityEntity MidtermActivityIPP = new()
    {
        Id = Guid.Parse("4b605875-edb5-4ce7-b4b9-b9230d673241"),
        Start = new DateTime(2024, 3, 11, 8, 30, 0),
        End = new DateTime(2024, 3, 11, 9, 0, 0),
        Room = Room.T10_005,
        ActivityType = ActivityType.Midterm,
        Description = null,
        SubjectId = SubjectSeeds.IPPSubject.Id,
        Subject = SubjectSeeds.IPPSubject
    };

    public static readonly ActivityEntity TestActivityIZU = new()
    {
        Id = Guid.Parse("49eacc2d-39e1-49a3-8f89-66c761bfffb1"),
        Start = new DateTime(2024, 3, 11, 8, 30, 0),
        End = new DateTime(2024, 3, 11, 9, 0, 0),
        Room = Room.T10_005,
        ActivityType = ActivityType.Test,
        Description = null,
        SubjectId = SubjectSeeds.IZUSubject.Id,
        Subject = SubjectSeeds.IZUSubject
    };

    public static readonly ActivityEntity LabActivityIPK = new()
    {
        Id = Guid.Parse("f99b7a16-6b9f-4c65-b3e7-3de1beb8da05"),
        Start = new DateTime(2024, 3, 11, 8, 30, 0),
        End = new DateTime(2024, 3, 11, 9, 0, 0),
        Room = Room.T10_005,
        ActivityType = ActivityType.Lab,
        Description = null,
        SubjectId = SubjectSeeds.IPKSubject.Id,
        Subject = SubjectSeeds.IPKSubject
    };

    public static void Seed(this ModelBuilder modelBuilder) =>
        modelBuilder.Entity<ActivityEntity>().HasData(
            ExamActivityICS with { Subject = null! },
            MidtermActivityICS with { Subject = null! },
            LectureActivityICS with { Subject = null! },
            MidtermActivityIPP with { Subject = null! },
            TestActivityIZU with { Subject = null! },
            LabActivityIPK with { Subject = null! }
        );
}