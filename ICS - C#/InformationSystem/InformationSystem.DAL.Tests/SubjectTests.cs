using InformationSystem.Common.Tests;
using InformationSystem.Common.Tests.Seeds;
using InformationSystem.DAL.Entities;
using InformationSystem.DAL.Enums;
using InformationSystem.DAL.Tests;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Internal;
using Xunit;
using Xunit.Abstractions;
using StudentSeeds = InformationSystem.DAL.Seeds.StudentSeeds;

namespace InformationSystem.DAL.Tests;

public class DbContextSubjectTests(ITestOutputHelper output) : DbContextTestsBase(output)
{
    [Fact]
    public async Task AddNew_SubjectWithoutStudentsAndActivities_Persisted()
    {
        //Arrange
        var entity = SubjectSeeds.EmptySubjectEntity with
        {
            Name = "Formalni jazyky a prekladace", Abbreviation = "IFJ"
        };

        //Act
        InformationSystemDbContextSUT.Subjects.Add(entity);
        await InformationSystemDbContextSUT.SaveChangesAsync();

        //Assert
        await using var dbx = await DbContextFactory.CreateDbContextAsync();
        var actualEntity = await dbx.Subjects
            .SingleAsync(i => i.Id == entity.Id);
        DeepAssert.Equal(entity, actualEntity);
    }

    [Fact]
    public async Task AddNew_SubjectWithActivity_Persisted()
    {
        //Arrange
        var entity = SubjectSeeds.EmptySubjectEntity with
        {
            Name = "Matematycká analíza 2",
            Abbreviation = "YMA2",
            Activities = new List<ActivityEntity>
            {
                ActivitySeeds.EmptyActivity with
                {
                    Start = new DateTime(2024, 03, 22),
                    End = new DateTime(2024, 03, 23),
                    Room = Room.E112,
                },
                ActivitySeeds.EmptyActivity with
                {
                    Start = new DateTime(2022, 05, 12),
                    End = new DateTime(2023, 1, 1),
                    Room = Room.D105,
                }
            }
        };
        
        //Act
        InformationSystemDbContextSUT.Subjects.Add(entity);
        await InformationSystemDbContextSUT.SaveChangesAsync();
        
        //Assert
        await using var dbx = await DbContextFactory.CreateDbContextAsync();
        var actualEntity = await dbx.Subjects
            .Include(i => i.Activities)
            .SingleAsync(i => i.Id == entity.Id);
        DeepAssert.Equal(entity, actualEntity);
    }


    [Fact]
    public async Task AddNew_StudentWithSubject_Persisted()
    {
        //Arrange
        var entity = StudentsInSubjectSeeds.EmptyStudentsInSubjectEntity with
        {
            Subject = SubjectSeeds.EmptySubjectEntity with
            {
                Name = "Matematycká analíza 2", Abbreviation = "YMA2",
            },
            Student = new StudentEntity { Name = "Michal", Surname = "Blizek", Login = "xblize69" }
        };
        
        //Act
        InformationSystemDbContextSUT.StudentsInSubject.Add(entity);
        await InformationSystemDbContextSUT.SaveChangesAsync();
        
        //Assert
        await using var dbx = await DbContextFactory.CreateDbContextAsync();
        var actualEntity = await dbx.StudentsInSubject
            .Include(i=>i.Student)
            .Include(i =>i.Subject)
            .SingleAsync(i => i.Student.Login == entity.Student.Login);
        DeepAssert.Equal(entity, actualEntity);
    }
}
