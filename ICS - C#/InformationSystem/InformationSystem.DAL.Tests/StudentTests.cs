using InformationSystem.Common.Tests;
using InformationSystem.Common.Tests.Seeds;
using InformationSystem.DAL.Entities;
using InformationSystem.DAL.Mappers;
using InformationSystem.DAL.Tests;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Internal;
using Xunit;
using Xunit.Abstractions;

namespace InformationSystem.DAL.Tests;

public class DbContextStudentTests(ITestOutputHelper output) : DbContextTestsBase(output)
{
    [Fact]
    public async Task AddNew_Student_Persisted()
    {
        //Arrange
        var entity = StudentSeeds.EmptyStudent with
        {
            Name = "Karel",
            Surname = "Kružík",
            Login = "xkruzi69",
            PhotoUrl = new Uri("https://example.com/")
        };
        
        //Act
        InformationSystemDbContextSUT.Students.Add(entity);
        await InformationSystemDbContextSUT.SaveChangesAsync();
        
        //Assert
        await using var dbx = await DbContextFactory.CreateDbContextAsync();
        var actualEntity = await dbx.Students
            .SingleAsync(i => i.Login == entity.Login);
        DeepAssert.Equal(entity, actualEntity);
    }

    [Fact]
    public async Task UpdateURL_Student_Updated()
    {
        //Arrange
        var entity = StudentSeeds.EmptyStudent with
        {
            Name = "Karel",
            Surname = "Kružík",
            Login = "xkruzi69",
            PhotoUrl = new Uri("https://example.com/")
        };
        
        //Act: Add to DB
        InformationSystemDbContextSUT.Students.Add(entity);
        await InformationSystemDbContextSUT.SaveChangesAsync();
        
        //Act: Update URL
        var studentToUpdate =
            await InformationSystemDbContextSUT.Students.SingleOrDefaultAsync(i => i.Login == entity.Login);

        if (studentToUpdate != null)
        {
            studentToUpdate.PhotoUrl = new Uri("https://vut.cz/");
            await InformationSystemDbContextSUT.SaveChangesAsync();
        }
        
        //Assert
        var updatedStudent =
            await InformationSystemDbContextSUT.Students.SingleOrDefaultAsync(i => i.Login == entity.Login);
        Assert.NotNull(updatedStudent);
        Assert.Equal(new Uri("https://vut.cz/"), updatedStudent!.PhotoUrl);
    }
    
    
    [Fact]
    public async Task DeleteByLogin_Student_Deleted()
    {
        //Arrange
        var entity = StudentSeeds.EmptyStudent with
        {
            Name = "Karel",
            Surname = "Kružík",
            Login = "xkruzi69",
            PhotoUrl = new Uri("https://example.com/")
        };
        
        //Act: Add student
        InformationSystemDbContextSUT.Students.Add(entity);
        await InformationSystemDbContextSUT.SaveChangesAsync();
        
        //Act: Delete student
        var studentToRemove =
            await InformationSystemDbContextSUT.Students.SingleOrDefaultAsync(i => i.Login == entity.Login);

        if (studentToRemove != null)
        {
            InformationSystemDbContextSUT.Students.Remove(studentToRemove);
            await InformationSystemDbContextSUT.SaveChangesAsync();
        }

        //Assert
        Assert.False(await InformationSystemDbContextSUT.Students.AnyAsync(i => i.Login == entity.Login));
    }

    [Fact]
    public async Task GetByIdWithoutStudentEval_Student()
    {
        //Act
        var entity = await InformationSystemDbContextSUT.Students
            .SingleAsync(i => i.Id == StudentSeeds.StudentEntity1.Id);
        
        //Assert
        DeepAssert.Equal(StudentSeeds.StudentEntity1, entity, [nameof(StudentEntity.ActivityEvaluation), nameof(StudentEntity.Subjects)]);
    }

}