using InformationSystem.Common.Tests;
using InformationSystem.Common.Tests.Factories;
using Microsoft.EntityFrameworkCore;
using Xunit;
using Xunit.Abstractions;

namespace InformationSystem.DAL.Tests;

public class  DbContextTestsBase : IAsyncLifetime
{
    protected DbContextTestsBase(ITestOutputHelper output)
    {
        XUnitTestOutputConverter converter = new(output);
        Console.SetOut(converter);

        DbContextFactory = new DbContextSqLiteTestingFactory(GetType().FullName!, seedTestingData: true);
        InformationSystemDbContextSUT = DbContextFactory.CreateDbContext();
    }

    protected IDbContextFactory<InformationSystemDbContext> DbContextFactory { get; }
    protected InformationSystemDbContext InformationSystemDbContextSUT { get; }


    public async Task InitializeAsync()
    {
        await InformationSystemDbContextSUT.Database.EnsureDeletedAsync();
        await InformationSystemDbContextSUT.Database.EnsureCreatedAsync();
    }

    public async Task DisposeAsync()
    {
        await InformationSystemDbContextSUT.Database.EnsureDeletedAsync();
        await InformationSystemDbContextSUT.DisposeAsync();
    }
}
